#include "tef/aurora/smartFuse.h"
#include <spdlog/spdlog.h>

#include <wiringPi.h>
#include <wiringSerial.h>

TEF::Aurora::SmartFuse::SmartFuse()
{
}

TEF::Aurora::SmartFuse::~SmartFuse()
{
	serialClose(m_serialPort);
}

bool TEF::Aurora::SmartFuse::Connect()
{
	m_serialPort = serialOpen("/dev/ttyUSB0", 57600);
	if (m_serialPort < 0)	/* open serial port */
	{
		spdlog::error("Unable to open serial device");
		return false;
	}

	wiringPiSetup();

	m_front = -1;

	return true;
}

bool TEF::Aurora::SmartFuse::SetFet(int channel, bool enabled)
{
	char c = channel * 2 + int(enabled);
	serialPutchar(m_serialPort, c);

	std::scoped_lock lock(m_stateMutex);
	m_fetStates[channel] = enabled;

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
		m_currentReadings[i] = current;
		m_fetStates[i] = fet;
	}

	return true;
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

