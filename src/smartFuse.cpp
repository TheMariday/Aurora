#include "tef/aurora/smartFuse.h"
#include <spdlog/spdlog.h>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

// General parameters
#define CHANNELS 8
#define TIMEOUT 1000
#define BAUD 57600

// Unique call codes
#define SERIAL_PING 100
#define SERIAL_STOP_ALL  101
#define SERIAL_GET_ALL  102

// Call indexes (to set fet 4 to on, call SERIAL_FET_ON+4)
#define SERIAL_FET_OFF CHANNELS*0
#define SERIAL_FET_ON  CHANNELS*1
#define SERIAL_GET     CHANNELS*2

// Response codes
#define RESP_SUCCESS 1
#define RESP_FAIL 0

TEF::Aurora::SmartFuse::SmartFuse()
{
	//set the number of channels
	m_channels.resize(8);
}

TEF::Aurora::SmartFuse::~SmartFuse()
{
	Stop();
	StopAll();
	close(m_serialPort);
}

bool TEF::Aurora::SmartFuse::Connect(std::string device)
{
	std::scoped_lock lock(m_fuseLock);
	m_serialPort = open(device.c_str(), O_RDWR);

	// Check for errors
	if (m_serialPort < 0) {
		spdlog::error("Smart Fuse failed to open serial port {:s}", device);
		return false;
	}

	struct termios tty;

	// Read in existing settings, and handle any error
	// NOTE: This is important! POSIX states that the struct passed to tcsetattr()
	// must have been initialized with a call to tcgetattr() overwise behaviour
	// is undefined
	if (tcgetattr(m_serialPort, &tty) != 0) {
		spdlog::error("Smart Fuse failed to get termios settings for port {:s}", device);
		return false;
	}


	tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
	tty.c_cflag &= ~CSIZE; // Clear all the size bits, then use one of the statements below
	tty.c_cflag |= CS8; // 8 bits per byte (most common)
	tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
	tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO; // Disable echo
	tty.c_lflag &= ~ECHOE; // Disable erasure
	tty.c_lflag &= ~ECHONL; // Disable new-line echo
	tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling of received bytes
	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
	tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	tty.c_cc[VMIN] = 0;

	cfsetispeed(&tty, B57600);
	cfsetospeed(&tty, B57600);

	// Save tty settings, also checking for error
	if (tcsetattr(m_serialPort, TCSANOW, &tty) != 0) {
		spdlog::error("Smart Fuse failed to set termios settings for port {:s}", device);
		return false;
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));

	if (tcflush(m_serialPort, TCIOFLUSH) != 0)
	{
		spdlog::error("Smart Fuse failed to flush stream after connecting for port {:s}", device);
		return false;
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));

	m_connected = true;

	if (!Ping())
	{
		spdlog::error("Smart Fuse connected but failed to ping check on port {:s}", device);
		m_connected = false;
		return false;
	}

	return true;
}

bool TEF::Aurora::SmartFuse::SetFet(unsigned int channel, bool fetState)
{
	std::scoped_lock lock(m_fuseLock);
	
	if (!CheckChannel(channel))
	{
		spdlog::error("Smart Fuse channel {} cannot set fet as channel isn't avaliable");
		return false;
	}

	// do not enable any fets if system is disabled
	if (!m_enabled && fetState)
	{
		spdlog::error("Smart Fuse channel {} cannot set fet as smart fuse is disabled", channel);
		return false;
	}

	if (!Write((unsigned char)(channel + (fetState ? SERIAL_FET_ON : SERIAL_FET_OFF))))
	{
		spdlog::error("Smart Fuse channel {} failed to set fet state", channel);
		return false;
	}

	int response = Read(); // this returns the post fet opening value, disregarding for now

	if (response == RESP_FAIL)
	{
		spdlog::error("Smart Fuse channel {} hardware failed to set fet", channel);
		return false;
	}

	m_channels[channel].fetState = fetState;

	return true;
}


bool TEF::Aurora::SmartFuse::GetCurrentRaw(unsigned int channel, unsigned int& current)
{
	std::scoped_lock lock(m_fuseLock);

	if (!CheckChannel(channel))
	{
		spdlog::error("Smart Fuse channel {} failed to get current", channel);
		return false;
	}

	if (!Write((unsigned char)(channel + SERIAL_GET)))
	{
		spdlog::error("Smart Fuse channel {} failed to write to the hardware requesting channel current", channel);
		return false;
	}

	current = Read();

	if (current == RESP_FAIL)
	{
		spdlog::error("Smart Fuse channel {} failed to get the raw current", channel);
		return false;
	}

	return true;
}

bool TEF::Aurora::SmartFuse::isCalibrated(unsigned int channel)
{
	return m_channels[channel].calibrated;
}

bool TEF::Aurora::SmartFuse::GetCurrent(unsigned int channel, float& current, bool openFet)
{
	if (!CheckChannel(channel))
	{
		spdlog::error("Smart Fuse channel {} failed to get current", channel);
		return false;
	}

	// do we need to open / close the fet?
	bool forceFetOpen = openFet && ! m_channels[channel].fetState;

	if (forceFetOpen)
	{
		if (!SetFet(channel, true))
		{
			spdlog::error("Smart Fuse channel {} failed to get current as the fet cannot be opened", channel);
			return false;
		};
	}

	unsigned int currentRaw;

	if (!GetCurrentRaw(channel, currentRaw))
	{
		spdlog::error("Smart Fuse channel {} failed to get current after fet had been opened", channel);
		return false;
	};

	current = MeasurementToAmps(channel, currentRaw);

	if (forceFetOpen)
	{
		if (!SetFet(channel, false))
		{
			spdlog::error("Smart Fuse channel {} failed to get current as the fet cannot be opened", channel);
			return false;
		};
	}

	return true;
}

bool TEF::Aurora::SmartFuse::GetCurrents(std::vector<float>& currents)
{
	std::scoped_lock lock(m_fuseLock);

	if (!Write(SERIAL_GET_ALL))
	{
		spdlog::error("Smart Fuse failed to get all currents");
		return false;
	};

	currents.resize(CHANNELS);
	for (unsigned char i = 0; i < 8; ++i)
	{
		int response = Read();

		if (response != RESP_FAIL)
		{
			currents[i] = MeasurementToAmps(i, response);
		}
		else
		{
			spdlog::error("Smart Fuse failed to read a board current");
			return false;
		}
	}

	return true;
};


bool TEF::Aurora::SmartFuse::StopAll()
{
	std::scoped_lock lock(m_fuseLock);

	if (!IsConnected())
	{
		spdlog::error("Smart Fuse cannot stop all fuses as hardware is not connected");
		return false;
	}

	if (!Write(SERIAL_STOP_ALL))
	{
		spdlog::error("Smart Fuse failed to write stop all command");
		return false;
	}

	if (Read() != RESP_SUCCESS)
	{
		spdlog::error("Smart Fuse returned an unexpected response when trying to stop all fuses");
		return false;
	}

	for (Channel& c : m_channels)
		c.fetState = false;

	return true;
}

bool TEF::Aurora::SmartFuse::Calibrate(unsigned int channel, unsigned int measurementZero, float measurementScale)
{
	if (channel >= m_channels.size())
	{
		spdlog::error("Smart Fuse channel {} cannot set calibration as it is outside of expected range (0-{})", channel, m_channels.size());
		return false;
	}

	m_channels[channel].zero = measurementZero;
	m_channels[channel].scale = measurementScale;
	m_channels[channel].calibrated = true;

	return true;
}

bool TEF::Aurora::SmartFuse::CheckConnected()
{
	Ping();

	for (unsigned int channel = 0; channel < m_channels.size(); channel++)
	{
		float current = 0;

		// if the channel isn't calibrated, don't check it
		if (!m_channels[channel].calibrated)
			continue;

		if (!GetCurrent(channel, current, true))
		{
			spdlog::error("Smart Fuse channel {} failed to get current whilst checking connected", channel);
			return false;
		}

		bool isConnected = current > m_minimumChannelAmperage; // 200mah

		if (isConnected != m_channels[channel].connected)
		{
			spdlog::info("Smart Fuse channel {0} has detected a channel current change of {1:.8f}A", channel, (current - m_minimumChannelAmperage));
			m_channels[channel].connected = isConnected;

			//there must be a difference
			if (isConnected == true)
			{
				std::stringstream ss;
				ss << "power channel " << channel << " has connected";
				Error e(ErrorType::Electrical, ErrorLevel::Warning, ss.str());
				Report(e);
			}

			if (isConnected == false)
			{
				std::stringstream ss;
				ss << "power channel " << channel << " has disconnected";
				Error e(ErrorType::Electrical, ErrorLevel::Critical, ss.str());
				Report(e);
			}
		}

	}
	return true;
}

int TEF::Aurora::SmartFuse::Read()
{
	if (!m_connected)
	{
		spdlog::error("Smart Fuse cannot be read as smart fuse is not connected");
		return RESP_FAIL;
	}

	char read_buf[2];
	int n = read(m_serialPort, &read_buf, sizeof(read_buf));

	if (n != 2)
	{
		spdlog::error("Smart Fuse failed to read the full 2 bytes needed");
		return RESP_FAIL;
	}

	return read_buf[1] << 8 | read_buf[0];
}

bool TEF::Aurora::SmartFuse::MainLoopCallback()
{
	CheckConnected();
	return true;
}

bool TEF::Aurora::SmartFuse::Ping()
{
	if (!Write(SERIAL_PING))
	{
		spdlog::error("Smart Fuse failed to send ping");
		return false;
	}
	return Read() == RESP_SUCCESS;
}

bool TEF::Aurora::SmartFuse::Write(unsigned char flag)
{
	if (!m_connected)
	{
		spdlog::error("Smart Fuse cannot be written to as hardware is not connected");
		return false;
	}

	unsigned char flagChar = flag;
	int bytesWritten = write(m_serialPort, &flagChar, 1);

	if (bytesWritten != 1)
	{
		spdlog::error("Smart Fuse failed to write data for some reason");
		return false;
	}

	return true;
}

float TEF::Aurora::SmartFuse::MeasurementToAmps(unsigned int channel, unsigned int measurement)
{
	if (!CheckChannel(channel))
	{
		spdlog::error("Smart Fuse channel {} cannot convert measurement", channel);
		return 0.0f;
	}

	return (static_cast<float>(measurement) - static_cast<float>(m_channels[channel].zero)) * m_channels[channel].scale;
}

bool TEF::Aurora::SmartFuse::CheckChannel(unsigned int channel)
{
	if (!m_connected)
	{
		spdlog::error("Smart Fuse is not connected", channel);
		return false;
	}

	if (channel >= m_channels.size())
	{
		spdlog::error("Smart Fuse failed to access channel {} as it is outside of expected range (0-{})", channel, m_channels.size());
		return false;
	}

	return true;
}
