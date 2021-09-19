#pragma once
#include <vector>
#include <string>
#include <functional>
#include "tef/aurora/runnable.h"


namespace TEF::Aurora {

	class SmartFuse : public Runnable
	{
	public:
		SmartFuse();
		~SmartFuse();

		bool Connect();

		bool Enable() { m_enabled = true; return true; };
		bool Disable() { m_enabled = false; return true; };

		bool SetFet(int channel, bool enabled, int& current);
		bool GetCurrent(int channel, int& current);
		bool GetCurrent(std::vector<int>& currents);
		bool StopAll();

		bool CheckConnected();

	private:

		bool MainLoopCallback() override;

		bool Ping();

		bool Write(int flag);
		int Read();

		float MeasurementToAmps(int measurement);

		bool m_enabled = true;

		int m_serialPort = -1;

		bool m_connected[8];
		bool m_enabledChannels[8];

	};

};
