#include "tef/aurora/batteryMonitor.h"

#include <spdlog/spdlog.h>

TEF::Aurora::BatteryMonitor::BatteryMonitor()
{
	SetFPS(1);
}

bool TEF::Aurora::BatteryMonitor::Connect(DacMCP3008* dac, voltage minVoltage, std::vector<int> cellPins)
{
	if (!dac)
	{
		spdlog::error("Battery Monitor initialised without a valid dac");
		return false;
	}

	if ((cellPins.size() == 0) || (cellPins.size() > dac->maxChannels()))
	{
		spdlog::error("Battery Monitor cannot be inintialised as there were too many / not enough pins to assign");
		return false;
	}

	if (minVoltage < 3)
	{
		spdlog::warn("Battery Monitor cannot set a minimum cell voltage to below 3v");
		minVoltage = 3;
	}

	for (int i = 0; i < cellPins.size(); i++)
	{
		Cell cell;
		cell.cellIndex = i;
		cell.sensePin = cellPins[i];
		cell.minimumVoltage = minVoltage;
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

		cell.currentVoltage = cumulativeActual - cumulativePrevious;

		cumulativePrevious = cumulativeActual;

		if ((cell.currentVoltage < 0) || (cell.currentVoltage > 5))
		{
			spdlog::error("Battery Monitor pins may have disconnected, reporting cell {} with a voltage of {}", cell.cellIndex, cell.currentVoltage);

			return false;
		}

		if (cell.currentVoltage < cell.minimumVoltage)
		{
			spdlog::warn("Battery Monitor sensed Cell {} is at {}v which is less than the minimum {}v", cell.cellIndex, cell.currentVoltage, cell.minimumVoltage);
			m_lowBatteryCallback(cell);
		}

		spdlog::debug("Battery Monitor Cell {} is at {}v", cell.cellIndex, cell.currentVoltage);
	}

	return false;
}

voltage TEF::Aurora::VoltageDivider::sensedToActual(voltage sensed)
{
	return (sensed * (R1 + R2)) / float(R2);
}
