#include "tef/aurora/smartFuse.h"
#include <spdlog/spdlog.h>

#include <wiringPi.h>
#include <wiringSerial.h>

TEF::Aurora::SmartFuse::SmartFuse()
{
}

TEF::Aurora::SmartFuse::~SmartFuse()
{
	StopAll();
	serialClose(m_serialPort);
}

bool TEF::Aurora::SmartFuse::Connect()
{
	wiringPiSetup();

	m_serialPort = serialOpen("/dev/ttyUSB0", 57600);
	if (m_serialPort < 0)	/* open serial port */
	{
		spdlog::error("Unable to open serial device");
		return false;
	}

	m_front = -1;

	std::this_thread::sleep_for(std::chrono::seconds(1));

	StopAll();

	return true;
}

bool TEF::Aurora::SmartFuse::SetFet(int channel, bool enabled)
{
	if (m_serialPort < 0)
	{
		spdlog::error("Smart Fuse cannot set fet as smart fuse is not connected");
		return false;
	}

	unsigned char c = channel * 2 + int(enabled);
	serialPutchar(m_serialPort, c);

	return true;
}

bool TEF::Aurora::SmartFuse::GetFet(int channel, bool& enabled)
{
	std::scoped_lock lock(m_stateMutex);
	enabled = m_fetStates[channel];
	return true;
}

bool TEF::Aurora::SmartFuse::GetCurrent(int channel, float& current)
{
	std::scoped_lock lock(m_stateMutex);
	current = m_currentReadings[channel];
	return true;
}

bool TEF::Aurora::SmartFuse::StopAll()
{
	for (int channel = 0; channel < m_channels; channel++)
	{
		if (!SetFet(channel, false))
		{
			spdlog::error("Smart Fuse cannot stop channel {}", channel);
		}

	}
	return true;
}


bool TEF::Aurora::SmartFuse::DecodeBuffer()
{
	std::scoped_lock lock(m_stateMutex);
	char lsb, msb;
	int current;
	bool fet;
	for (int i = 0; i < 8; i++)
	{
		lsb = m_charBuffer[3 * i + 0];
		msb = m_charBuffer[3 * i + 1];
		fet = m_charBuffer[3 * i + 2];
		current = (msb << 8 | lsb);
		m_currentReadings[i] = MeasurementToAmps(current, i != 7);
		m_fetStates[i] = fet;
	}

	return true;
}

float TEF::Aurora::SmartFuse::MeasurementToAmps(int measurement, bool asc10)
{
	if (asc10)
	{
		return -1;
	}
	else
	{
		return (measurement * 0.0579) - 24.6;
	}

}

bool TEF::Aurora::SmartFuse::Print()
{
	std::scoped_lock lock(m_stateMutex);

	for (int i = 0; i < 8; i++)
	{
		spdlog::debug("Sensor {}: Fet: {}, Current: {}", i, m_fetStates[i], m_currentReadings[i]);
	}
	return true;
}

bool TEF::Aurora::SmartFuse::MainLoopCallback()
{
	if (serialDataAvail(m_serialPort))
	{
		m_front++;

		char d = serialGetchar(m_serialPort);		/* receive character serially*/
		m_charBuffer[m_front] = d;

		if (m_front < 1)
			return true;

		if (m_charBuffer[m_front] == 255 and m_charBuffer[m_front - 1] == 255)
		{
			if (m_front == 25)
			{
				DecodeBuffer();
			}
			m_front = -1;
		}

	}
	else
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}

	return true;
}

