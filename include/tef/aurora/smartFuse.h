#pragma once
#include "tef/aurora/effect.h"

namespace TEF::Aurora {

	class SmartFuse : public Effect
	{
	public:
		SmartFuse();
		~SmartFuse();

		bool Connect();

		bool SetFet(int channel, bool enabled);
		bool GetFet(int channel, bool& enabled);
		bool GetCurrent(int channel, float& current);

		bool StopAll();

	private:

		bool MainLoopCallback() override;

		bool Write(std::string);


		const static int m_channels = 8;
		float m_currentReadings[m_channels];
		bool m_fetStates[m_channels];

		int m_serialPort;
	};
};
