#pragma once
#include <vector>
#include <string>
#include <functional>
#include "tef/aurora/runnable.h"


namespace TEF::Aurora {

	struct Calibration
	{
		bool calibrated = false;
		float scale = 0.025586f;
		int zero = 500;
	};

	class SmartFuse : public Runnable
	{
	public:
		SmartFuse();
		~SmartFuse();

		bool Connect(std::string device);

		bool Enable() { m_enabled = true; return true; };
		bool Disable() { m_enabled = false; return true; };

		bool SetFet(unsigned char channel, bool enabled);
		bool GetCurrent(unsigned char channel, float& current);
		bool GetCurrent(std::vector<float>& currents);
		bool GetCurrentRaw(unsigned char channel, int& current);

		bool isCalibrated(unsigned char channel);

		bool StopAll();

		bool Calibrate(unsigned char channel, int measurementZero, float measurementScale);

		bool CheckConnected();

	private:


		bool MainLoopCallback() override;

		bool Ping();

		bool Write(unsigned char flag);
		int Read();

		float MeasurementToAmps(unsigned char channel, int measurement);

		bool m_enabled = true;

		int m_serialPort = -1;

		const float m_minimumChannelAmperage = 0.2f; // 200mah / 200 leds / 4 strips

		bool m_channelConnected[8];
		bool m_enabledChannels[8];
		Calibration m_calibration[8];

	};

};
