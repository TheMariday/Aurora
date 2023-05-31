#include "tef/aurora/button.h"

#include <spdlog/spdlog.h>
#include <sstream>

#include "tef/aurora/error.h"



bool TEF::Aurora::DacButton::Connect(DacMCP3008* pDac, int pin, std::string name, int debounceTime) {

	if (pin < 4)
	{
		spdlog::error("DacButton cannot connect to pin {} as pin is not set up for button usage", pin);
		return false;
	}

	if (pin > 6)
	{
		spdlog::error("DacButton cannot connect to pin {} as pin is not set up for button usage", pin);
		return false;
	}

	if (!pDac->isConnected())
	{
		spdlog::warn("DacButton cannot connect to dac as dac is not connected, connecting dac");
		if (!pDac->Connect())
		{
			spdlog::error("DacButton failed to connect to unconnected dac");
			return false;
		}
	}

	m_pin = pin;
	m_pDac = pDac;

	m_debounce = std::chrono::microseconds(debounceTime);
	m_lastCallback = std::chrono::high_resolution_clock::now();

	//This is called manually here to ensure that the IsConnected call has the correct state
	MainLoopCallback();

	if (m_state != UP)
	{
		spdlog::error("Button started in an invalid state");
		return false;
	}

	m_connected = true;
	return true;
}

bool TEF::Aurora::DacButton::MainLoopCallback()
{
	float volts;
	m_pDac->Read(m_pin, volts);

	int previousState = m_state;
	m_state = VoltageToState(volts);

	std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();

	// if there's no state change, break
	if (m_state == previousState)
		return true;

	// if the time since last callback is too short, also break
	if (t < m_lastCallback + m_debounce)
		return true;

	// if previous state is disconnected, ignore any further state changes and report reconnect
	if (previousState == DISCONNECTED)
	{
		std::stringstream reconnectDetails;
		reconnectDetails << m_name << " button has reconnected";
		Error reconnectError = Error(ErrorType::Electrical, ErrorLevel::Warning, reconnectDetails.str());
		Report(reconnectError);
		std::this_thread::sleep_for(std::chrono::seconds(2)); // we sleep here because we don't want to get any weird debouncing issues when connecting / disconnecting
	}
	else
	{
		switch (m_state)
		{
		case(DISCONNECTED):
		{
			std::stringstream disconnectDetails;
			disconnectDetails << m_name << " button has disconnected";
			Error disconnectError = Error(ErrorType::Electrical, ErrorLevel::Critical, disconnectDetails.str());
			Report(disconnectError);
			std::this_thread::sleep_for(std::chrono::seconds(2)); // we sleep here because we don't want to get any weird debouncing issues when connecting / disconnecting
		}
		break;
		case(UP):
		{
			if (m_upCallback) m_upCallback();
		}
		break;
		case(DOWN):
		{
			if (m_downCallback) m_downCallback();
		}
		break;
		}
	}

	m_lastCallback = t;

	return true;
}

int TEF::Aurora::DacButton::VoltageToState(float volts)
{
	if (volts < 1.1)
		return DOWN;
	else
		return UP;
}
