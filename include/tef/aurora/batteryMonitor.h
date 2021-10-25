#pragma once

#include "tef/aurora/runnable.h"
#include "tef/aurora/dacMCP3008.h"

namespace TEF::Aurora {

	struct Cell
	{
		int cellIndex;
		int sensePin;
		float currentVoltage;
		float minimumVoltage;
	};

	class BatteryMonitor : public Runnable
	{
	public:
		BatteryMonitor();

		bool Connect(DacMCP3008* dac, std::vector<int> cellPins, float minVoltage);

		bool IsConnected();

		bool GetCells(std::vector<Cell>& cells);

		bool MainLoopCallback() override;
	private:

		float sensedVoltageToActual(float sensed);

		float m_r1 = 39'000;
		float m_r2 = 10'000;

		DacMCP3008* m_pDac;
		std::vector<Cell> m_cells;

		bool m_connected = false;
	};
}