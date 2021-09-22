#include "tef/aurora/dacMCP3008.h"

#include <spdlog/spdlog.h>
#include <wiringPi.h>
#include <mcp3004.h>

TEF::Aurora::DacMCP3008::DacMCP3008()
{

}

bool TEF::Aurora::DacMCP3008::Connect(int spiChannel)
{
	if (wiringPiSetup() == -1)
	{
		spdlog::warn("DacMCP3008 might have failed to initialise wiringpi, continuing regardless");
		//return false;
	}

	if (!mcp3004Setup(m_channelOffset, spiChannel))
	{
		spdlog::warn("DacMCP3008 might have failed to initialise the MCP, continuing regardless");
		//return false;
	}

	m_connected = true;

	return true;
}

bool TEF::Aurora::DacMCP3008::isConnected()
{
	return m_connected;
}

bool TEF::Aurora::DacMCP3008::Read(int channel, voltage& volts)
{
	int value = analogRead(m_channelOffset + channel);

	volts = (float(value) / float(m_maxValue)) * m_maxVoltage;
	
	return true;
}
