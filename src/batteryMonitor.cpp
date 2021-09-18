#include "tef/aurora/batteryMonitor.h"

#include <spdlog/spdlog.h>

TEF::Aurora::BatteryMonitor::BatteryMonitor()
{
	SetFPS(Settings::FPS_BATTERY);
}

bool TEF::Aurora::BatteryMonitor::Connect(DacMCP3008* dac)
{
	if (!dac)
	{
		spdlog::error("Battery Monitor initialised without a valid dac");
		return false;
	}

	std::vector<int> cellPins = { Settings::PIN_CELL_0, Settings::PIN_CELL_1, Settings::PIN_CELL_2, Settings::PIN_CELL_3 };

	for (int i = 0; i < cellPins.size(); i++)
	{
		Cell cell;
		cell.cellIndex = i;
		cell.sensePin = cellPins[i];
		cell.minimumVoltage = Settings::VOLTAGE_CELL_MIN;
		cell.currentVoltage = -1;
		m_cells.push_back(cell);
	}

	m_lowBatteryCallback = [](Cell cell) {spdlog::debug("No low battery callback set"); };

	m_pDac = dac;

	m_connected = true;

	return true;
}

bool TEF::Aurora::BatteryMonitor::IsConnected()
{
	return m_connected;
}

bool TEF::Aurora::BatteryMonitor::SetLowBatteryCallback(std::function<void(Cell)> func)
{
	m_lowBatteryCallback = func;
	return true;
}

bool TEF::Aurora::BatteryMonitor::GetCells(std::vector<Cell>& cells)
{
	cells = m_cells;
	return true;
}

bool TEF::Aurora::BatteryMonitor::MainLoopCallback()
{
	if (!m_pDac->isConnected())
	{
		spdlog::error("Battery Monitor tried to read from a disconnected dac");
		return false;
	}

	// this strangeness is because the voltage across line 0->1 = cell 1, but 0->2 is cell 1+2, 0->3 = cell 1+2+3 etc
	voltage cumulativePrevious = 0;

	for (Cell& cell : m_cells)
	{
		voltage cumulativeSensed, cumulativeActual;
		m_pDac->Read(cell.sensePin, cumulativeSensed);
		cumulativeActual = m_voltageDivider.sensedToActual(cumulativeSensed);

		if (cumulativeActual < 1)
		{
			spdlog::warn("Battery Monitor pin {} disconnected", cell.sensePin);
			return false;
		}

		cell.currentVoltage = cumulativeActual - cumulativePrevious;

		cumulativePrevious = cumulativeActual;

		if (cell.currentVoltage < cell.minimumVoltage)
		{
			spdlog::warn("Battery Monitor sensed Cell {} is at {}v which is less than the minimum {}v", cell.cellIndex, cell.currentVoltage, cell.minimumVoltage);
			m_lowBatteryCallback(cell);
		}
	}

	for (Cell& cell : m_cells)
	{
		spdlog::debug("Battery Monitor Cell {} is at {}v", cell.cellIndex, cell.currentVoltage);
	}

	return true;
}

voltage TEF::Aurora::VoltageDivider::sensedToActual(voltage sensed)
{
	return (sensed * (R1 + R2)) / float(R2);
}
