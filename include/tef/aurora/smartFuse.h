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

		bool CheckConnected(std::vector<bool>& connected);

	private:

		bool Ping();

		bool Write(int flag);
		int Read();

		float MeasurementToAmps(int measurement);

		int m_serialPort = -1;

	};
};
