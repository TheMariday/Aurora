#pragma once
#include <vector>
#include <string>

namespace TEF::Aurora {

	class SmartFuse
	{
	public:
		SmartFuse();
		~SmartFuse();

		bool Connect(std::string port = "/dev/ttyUSB0");

		bool SetFet(int channel, bool enabled, int& current);
		bool GetCurrent(int channel, int& current);
		bool GetCurrent(std::vector<int>& currents);
		bool StopAll();

	private:

		int Read();

		float MeasurementToAmps(int measurement);

		const static int m_channels = 8;

		int m_serialPort;

		char m_charBuffer[50];

	};
};
