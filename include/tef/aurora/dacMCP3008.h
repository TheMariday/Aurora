#pragma once
#define voltage float
#define ohms int

namespace TEF::Aurora {

	class DacMCP3008
	{
	public:
		DacMCP3008();

		bool Connect(int spiChannel = 0);

		bool isConnected();

		bool Read(int channel, voltage& volts);

		int maxChannels() { return 8; };

	private:
		bool m_connected = false;

		const int m_channelOffset = 200;
		const voltage m_maxVoltage = 3.3;
		const int m_maxValue = 1024;

	};
}