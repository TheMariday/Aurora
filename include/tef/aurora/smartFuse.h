#pragma once
#include <vector>
#include <string>
#include <functional>
#include "tef/aurora/runnable.h"


namespace TEF::Aurora {

	struct Channel
	{
		bool calibrated = false;
		float scale = 0.025586f;
		unsigned int zero = 500;
		bool connected = false;
		bool fetState = false;
	};

	class SmartFuse : public Runnable
	{
	public:
		SmartFuse();
		~SmartFuse();

		bool Connect(std::string device);
		bool Enable() { m_enabled = true; return true; };
		bool Disable() { m_enabled = false; return true; };

		bool SetFet(unsigned int channel, bool enabled);
		bool GetCurrent(unsigned int channel, float& current, bool openFet = false);
		bool GetCurrents(std::vector<float>& currents);

		bool isCalibrated(unsigned int channel);

		bool StopAll();

		bool Calibrate(unsigned int channel, unsigned int measurementZero, float measurementScale);

		bool CheckConnected();

	private:

		bool GetCurrentRaw(unsigned int channel, unsigned int& current);

		bool MainLoopCallback() override;

		bool Ping();

		bool Write(unsigned char flag);
		int Read();

		float MeasurementToAmps(unsigned int channel, unsigned int measurement);

		bool CheckChannel(unsigned int channel);

		bool m_enabled = true;

		int m_serialPort = -1;

		const float m_minimumChannelAmperage = 0.2f; // 200mah / 200 leds / 4 strips

		std::vector<Channel> m_channels;

		std::mutex m_fuseLock;

	};

};
