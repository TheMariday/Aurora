#pragma once

namespace TEF::Aurora {

	class DacMCP3008
	{
	public:
		DacMCP3008();

		bool Connect(int spiChannel=0);

		bool Read(int channel, int& value);

	private:

		int m_channelOffset = 200;

	};
}