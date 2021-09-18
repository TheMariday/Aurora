#include "tef/aurora/smartFuse.h"
#include <spdlog/spdlog.h>
#include "tef/aurora/settings.h"
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
}

TEF::Aurora::SmartFuse::~SmartFuse()
{
	StopAll();
	close(m_serialPort);
}

bool TEF::Aurora::SmartFuse::Connect()
{
	m_serialPort = open(Settings::DEVICE_FUSE.c_str(), O_RDWR);

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

	return true;
}

bool TEF::Aurora::SmartFuse::SetFet(int channel, bool enabled, int& current)
{
	if (m_serialPort < 0)
	{
		spdlog::error("Smart Fuse cannot set fet as smart fuse is not connected");
		return false;
	}

	Write((enabled ? SERIAL_FET_ON : SERIAL_FET_OFF) + channel);

	current = Read();

	return true;
}


bool TEF::Aurora::SmartFuse::GetCurrent(int channel, int& current)
{
	if (m_serialPort < 0)
	{
		spdlog::error("Smart Fuse cannot get the current as smart fuse is not connected");
		return false;
	}

	Write(SERIAL_GET + channel);

	current = Read();
	return true;
}

bool TEF::Aurora::SmartFuse::GetCurrent(std::vector<int>& currents)
{
	if (m_serialPort < 0)
	{
		spdlog::error("Smart Fuse cannot get the current as smart fuse is not connected");
		return false;
	}

	Write(SERIAL_GET_ALL);
	currents.resize(CHANNELS);
	for (int& current : currents)
	{
		current = Read();
		if (current == 0)
		{
			spdlog::error("Smart Fuse failed to read a board current");
			return false;
		}
	}

	return true;
};


bool TEF::Aurora::SmartFuse::StopAll()
{
	if (m_serialPort < 0)
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

	return true;
}

bool TEF::Aurora::SmartFuse::CheckConnected(std::vector<bool>& connected)
{
	connected.clear();
	connected.resize(CHANNELS);
	for (int channel = 0; channel < CHANNELS; channel++)
	{
		int currentBefore, currentAfter;
		SetFet(channel, false, currentBefore);
		SetFet(channel, true, currentAfter);

		connected[channel] = currentAfter > currentBefore + 5; // roughly 100ma
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

bool TEF::Aurora::SmartFuse::Ping()
{
	Write(SERIAL_PING);
	return Read() == 1;
}

bool TEF::Aurora::SmartFuse::Write(int flag)
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

float TEF::Aurora::SmartFuse::MeasurementToAmps(int measurement)
{
	//This needs to be calibratable
	return (measurement - 494.8) * (25 / 1024); // Which is roughly 20ma per step
}