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
}

TEF::Aurora::SmartFuse::~SmartFuse()
{
	StopAll();
	close(m_serialPort);
}

bool TEF::Aurora::SmartFuse::Connect(std::string port)
{
	m_serialPort = open(port.c_str(), O_RDWR);

	// Check for errors
	if (m_serialPort < 0) {
		printf("Error connecting\n");
	}

	printf("connected, sleeping\n");


	struct termios tty;

	// Read in existing settings, and handle any error
	// NOTE: This is important! POSIX states that the struct passed to tcsetattr()
	// must have been initialized with a call to tcgetattr() overwise behaviour
	// is undefined
	if (tcgetattr(m_serialPort, &tty) != 0) {
		printf("Error %i from tcgetattr: %i\n", errno, errno);
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
		printf("Error %i from tcsetattr", errno);
	}

	printf("connected, sleeping\n");

	std::this_thread::sleep_for(std::chrono::seconds(1));

	tcflush(m_serialPort, TCIOFLUSH);

	std::this_thread::sleep_for(std::chrono::seconds(1));

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
		spdlog::error("Smart Fuse cannot stop as smart fuse is not connected");
		return false;
	}

	Write(SERIAL_STOP_ALL);
	return Read() == RESP_SUCCESS;
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

	return read_buf[1] << 8 | read_buf[0];
}

int TEF::Aurora::SmartFuse::Write(int flag)
{
	if (m_serialPort < 0)
	{
		spdlog::error("Smart Fuse cannot be written to as smart fuse is not connected");
		return 0;
	}

	unsigned char flagChar = flag;
	int bytesWritten = write(m_serialPort, &flagChar, 1);
	return bytesWritten == 1;
}


float TEF::Aurora::SmartFuse::MeasurementToAmps(int measurement)
{
	return (measurement - 494.8) * (25 / 1024);
}