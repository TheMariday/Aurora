#pragma once
#include <functional>

#include "tef/aurora/runnable.h"
#include "tef/aurora/dacMCP3008.h"
#include "tef/aurora/settings.h"

namespace TEF::Aurora {

	struct VoltageDivider
	{
		ohms R1 = Settings::DIVIDER_OHM_1;
		ohms R2 = Settings::DIVIDER_OHM_2;

		voltage sensedToActual(voltage sensed);
	};

	struct Cell
	{
		int cellIndex;
		int sensePin;
		voltage currentVoltage;
		voltage minimumVoltage;
	};

	class BatteryMonitor : public Runnable
	{
	public:
		BatteryMonitor();

		bool Connect(DacMCP3008* dac);

		bool IsConnected();

		bool GetCells(std::vector<Cell>& cells);

		bool MainLoopCallback() override;
	private:
		DacMCP3008* m_pDac;
		std::vector<Cell> m_cells;

		VoltageDivider m_voltageDivider;

		bool m_connected = false;
	};
}