#include "tef/aurora/batteryMonitor.h"
#include <spdlog/spdlog.h>

TEF::Aurora::BatteryMonitor::BatteryMonitor()
{
}

bool TEF::Aurora::BatteryMonitor::Connect(DacMCP3008* dac, std::vector<int> cellPins, float minVoltage)
{
	if (!dac)
	{
		spdlog::error("Battery Monitor initialised without a valid dac");
		return false;
	}

	for (unsigned int i = 0; i < cellPins.size(); i++)
	{
		Cell cell;
		cell.cellIndex = i;
		cell.sensePin = cellPins[i];
		cell.minimumVoltage = minVoltage;
		cell.currentVoltage = -1;
		m_cells.push_back(cell);
	}

	m_pDac = dac;

	m_connected = true;

	return true;
}

bool TEF::Aurora::BatteryMonitor::IsConnected()
{
	return m_connected;
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
	float cumulativePreviousVoltage = 0;

	for (Cell& cell : m_cells)
	{
		float cumulativeSensedVoltage;
		if (!m_pDac->Read(cell.sensePin, cumulativeSensedVoltage))
		{
			spdlog::error("failed to read sense pin {}", cell.sensePin);
			return false;
		};

		float cumulativeActualVoltage = sensedVoltageToActual(cumulativeSensedVoltage);

		if (cumulativeActualVoltage < 1)
		{
			std::stringstream cellDisconnect;
			cellDisconnect << "Cell " << cell.cellIndex << " has disconnected";
			Error cellDisconnectError(ErrorType::Battery, ErrorLevel::Critical, cellDisconnect.str());
			Report(cellDisconnectError);
			return false;
		}

		cell.currentVoltage = cumulativeActualVoltage - cumulativePreviousVoltage;

		cumulativePreviousVoltage = cumulativeActualVoltage;

		if (cell.currentVoltage < cell.minimumVoltage)
		{
			std::stringstream lowCell;
			lowCell << "Cell " << cell.cellIndex << " is low. Cell reads " << cell.currentVoltage << " volts";
			Error lowCellError(ErrorType::Battery, ErrorLevel::Warning, lowCell.str());
			Report(lowCellError);
		}
	}

	return true;
}

float TEF::Aurora::BatteryMonitor::sensedVoltageToActual(float sensedVoltage)
{
	return (sensedVoltage * (float)(m_r1 + m_r2)) / float(m_r2);
}
