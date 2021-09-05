#pragma once
#include <functional>

#include "tef/aurora/runnable.h"
#include "tef/aurora/dacMCP3008.h"

namespace TEF::Aurora {

	struct VoltageDivider // default 16.17 -> 3.3v divider
	{
		ohms R1 = 39'000;
		ohms R2 = 10'000;

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
		BatteryMonitor(DacMCP3008* dac, voltage minVoltage = 3.2, std::vector<int> cellPins = { 0, 1, 2, 3 });

		bool SetLowBatteryCallback(std::function<void(Cell)> func);

		bool GetCells(std::vector<Cell>& cells);

		bool MainLoopCallback() override;
	private:
		DacMCP3008* m_pDac;
		std::vector<Cell> m_cells;

		VoltageDivider m_voltageDivider;

		std::function<void(Cell)> m_lowBatteryCallback;
	};
}