#include "tef/aurora/smartFuse.h"
#include <spdlog/spdlog.h>

#include <wiringPi.h>
#include <wiringSerial.h>

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
	serialClose(m_serialPort);
}

bool TEF::Aurora::SmartFuse::Connect(std::string port)
{
	wiringPiSetup();

	m_serialPort = serialOpen(port.c_str(), BAUD);
	if (m_serialPort < 0)	/* open serial port */
	{
		spdlog::error("Unable to open serial device");
		return false;
	}

	std::this_thread::sleep_for(std::chrono::seconds(2));

	return true;
}
bool TEF::Aurora::SmartFuse::SetFet(int channel, bool enabled, int& current)
{
	if (m_serialPort < 0)
	{
		spdlog::error("Smart Fuse cannot set fet as smart fuse is not connected");
		return false;
	}

	serialPutchar(m_serialPort, (enabled ? SERIAL_FET_ON : SERIAL_FET_OFF) + channel);

	return true;
}


bool TEF::Aurora::SmartFuse::GetCurrent(int channel, int& current)
{
	if (m_serialPort < 0)
	{
		spdlog::error("Smart Fuse cannot get the current as smart fuse is not connected");
		return false;
	}

	serialPutchar(m_serialPort, SERIAL_GET + channel);

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

	serialPutchar(m_serialPort, SERIAL_GET_ALL);
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

	serialPutchar(m_serialPort, SERIAL_STOP_ALL);
	return Read() == 1;
}

int TEF::Aurora::SmartFuse::Read()
{
	if (m_serialPort < 0)
	{
		spdlog::error("Smart Fuse cannot be read as smart fuse is not connected");
		return 0;
	}

	char lsb = 0;
	char msb = 0;

	lsb = serialGetchar(m_serialPort);
	msb = serialGetchar(m_serialPort);

	if ((lsb == -1) || (msb == -1))
	{
		spdlog::error("Smart Fuse failed to read a byte");
		return 0;
	}

	return msb << 8 | lsb;
}

float TEF::Aurora::SmartFuse::MeasurementToAmps(int measurement)
{
	return (measurement - 494.8) * (25 / 1024);
}