#pragma once
#include "tef/aurora/runnable.h"
#include <mutex>
#include <atomic>

namespace TEF::Aurora {

	class SmartFuse : public Runnable
	{
	public:
		SmartFuse();
		~SmartFuse();

		bool Connect();

		bool SetFet(int channel, bool enabled);
		bool GetFet(int channel, bool& enabled);
		bool GetCurrent(int channel, float& current);

		bool StopAll();

		bool Print();

	private:

		bool MainLoopCallback() override;

		bool DecodeBuffer();


		const static int m_channels = 8;
		float m_currentReadings[m_channels];
		bool m_fetStates[m_channels];

		std::mutex m_stateMutex;

		int m_serialPort;

		char m_charBuffer[50];
		int m_front;

	};
};
