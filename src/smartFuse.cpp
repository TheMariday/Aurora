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
	for (bool& v : m_enabledChannels)
		v = false;
	for (bool& v : m_channelConnected)
		v = false;
}

TEF::Aurora::SmartFuse::~SmartFuse()
{
	Stop();
	StopAll();
	close(m_serialPort);
}

bool TEF::Aurora::SmartFuse::Connect(std::string device)
{
	m_serialPort = open(device.c_str(), O_RDWR);

	// Check for errors
	if (m_serialPort < 0) {
		spdlog::error("Smart Fuse failed to connect to hardware");
		return false;
	}

	struct termios tty;

	// Read in existing settings, and handle any error
	// NOTE: This is important! POSIX states that the struct passed to tcsetattr()
	// must have been initialized with a call to tcgetattr() overwise behaviour
	// is undefined
	if (tcgetattr(m_serialPort, &tty) != 0) {
		spdlog::error("Smart Fuse failed to get termios settings");
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
		spdlog::error("Smart Fuse failed to set termios settings");
		return false;
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));

	if (tcflush(m_serialPort, TCIOFLUSH) != 0)
	{
		spdlog::error("Smart Fuse failed to flush stream after connecting");
		return false;
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));

	if (!Ping())
	{
		spdlog::error("Smart Fuse connected but failed ping check");
		return false;
	}

	m_connected = true;

	return true;
}

bool TEF::Aurora::SmartFuse::SetFet(unsigned char channel, bool enabled)
{
	if (m_serialPort < 0)
	{
		spdlog::error("Smart Fuse cannot set fet as smart fuse is not connected");
		return false;
	}

	// do not enable any fets if system is disabled
	if (!m_enabled && enabled)
	{
		spdlog::error("Smart Fuse cannot open fet as smart fuse is disabled");
		return false;
	}

	Write((unsigned char)(channel + (enabled ? SERIAL_FET_ON : SERIAL_FET_OFF)));
	m_enabledChannels[channel] = enabled;

	int response = Read(); // this returns the post fet opening value, disregarding for now

	if (response == RESP_FAIL)
	{
		spdlog::error("Smart Fuse failed to set Fet for channel {}", static_cast<int>(channel));
		return false;
	}

	return true;
}


bool TEF::Aurora::SmartFuse::GetCurrentRaw(unsigned char channel, int& current)
{
	if (m_serialPort < 0)
	{
		spdlog::error("Smart Fuse cannot get the current as smart fuse is not connected");
		return false;
	}

	Write((unsigned char)(channel + SERIAL_GET));

	current = Read();

	if (current == RESP_FAIL)
	{
		spdlog::error("Smart Fuse failed to read current from channel {}", static_cast<int>(channel));
		return false;
	}

	return true;
}

bool TEF::Aurora::SmartFuse::isCalibrated(unsigned char channel)
{
	return m_calibration[channel].calibrated;
}

bool TEF::Aurora::SmartFuse::GetCurrent(unsigned char channel, float& current)
{
	int c;
	if (!GetCurrentRaw(channel, c))
	{
		spdlog::error("SmartFuse cannot get channel current in amps as GetCurrent has failed");
		return false;
	}

	current = MeasurementToAmps(channel, c);

	return true;
}

bool TEF::Aurora::SmartFuse::GetCurrent(std::vector<float>& currents)
{
	if (m_serialPort < 0)
	{
		spdlog::error("Smart Fuse cannot get the current as smart fuse is not connected");
		return false;
	}

	Write(SERIAL_GET_ALL);
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

	for (bool& v : m_enabledChannels)
		v = false;

	return true;
}

bool TEF::Aurora::SmartFuse::Calibrate(unsigned char channel, int measurementZero, float measurementScale)
{
	m_calibration[channel].zero = measurementZero;
	m_calibration[channel].scale = measurementScale;
	m_calibration[channel].calibrated = true;

	return true;
}

bool TEF::Aurora::SmartFuse::CheckConnected()
{
	for (unsigned char channel = 0; channel < CHANNELS; channel++)
	{
		float current = 0;

		if (m_enabledChannels[channel])
		{
			GetCurrent(channel, current);
		}
		else
		{
			SetFet(channel, true);
			GetCurrent(channel, current);
			SetFet(channel, false);
		}
		bool connected = current > m_minimumChannelAmperage; // 200mah

		if (connected != m_channelConnected[channel])
		{
			m_channelConnected[channel] = connected;
			//there must be a difference
			if (connected == true)
			{
				std::stringstream ss;
				ss << "power channel " << channel << " has connected";
				Error e(ErrorType::Electrical, ErrorLevel::Warning, ss.str());
				Report(e);
			}

			if (connected == false)
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
	if (m_serialPort < 0)
	{
		spdlog::error("Smart Fuse cannot be read as smart fuse is not connected");
		return 0;
	}

	char read_buf[2];
	int n = read(m_serialPort, &read_buf, sizeof(read_buf));

	if (n != 2)
	{
		spdlog::error("Smart Fuse failed to read the full 2 bytes needed");
		return 0;
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
	Write(SERIAL_PING);
	return Read() == RESP_SUCCESS;
}

bool TEF::Aurora::SmartFuse::Write(unsigned char flag)
{
	if (m_serialPort < 0)
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

float TEF::Aurora::SmartFuse::MeasurementToAmps(unsigned char channel, int measurement)
{
	return static_cast<float>(measurement - m_calibration[channel].zero) * m_calibration[channel].scale;
}