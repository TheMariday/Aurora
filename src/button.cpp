#include "tef/aurora/button.h"

#include <spdlog/spdlog.h>
#include <sstream>

#include "tef/aurora/error.h"

bool TEF::Aurora::Button::Connect(int pin, int debounce) {
	std::stringstream ss;
	ss << "gpio export " << pin << " in";
	spdlog::debug("calling system {}", ss.str());
	system(ss.str().c_str());

	m_pin = pin;

	wiringPiSetup(); // This doesn't have a return code and just calls quit() if it fails and I hate it.

	pinMode(m_pin, INPUT);
	m_debounce = std::chrono::microseconds(debounce);

	m_lastCallback = std::chrono::high_resolution_clock::now();

	return true;
}

bool TEF::Aurora::Button::IsConnected()
{
	return m_pin != -1;
}

bool TEF::Aurora::Button::MainLoopCallback()
{
	int state = digitalRead(m_pin);

	std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();

	if ((t > m_lastCallback + m_debounce) and (state != m_state))
	{
		m_state = state;

		switch (m_state)
		{
		case(0):
			if (m_upCallback) m_upCallback();
			break;
		case(1):
			if (m_downCallback) m_downCallback();
			break;
		}
	}

	m_lastCallback = t;

	return true;
}

TEF::Aurora::DacButton::DacButton()
{

}

bool TEF::Aurora::DacButton::Connect(DacMCP3008* pDac, int pin, int debounceTime) {

	if (pin < 5)
	{
		spdlog::error("DacButton cannot connect to pin {} as pin is not set up for button usage", pin);
		return false;
	}

	if (pin > 7)
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

	return true;
}

bool TEF::Aurora::DacButton::IsConnected()
{
	return m_state != DISCONNECTED;
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
	if (volts > 2.2)
		return DOWN;
	else if (volts > 1.1)
		return UP;
	else
		return DISCONNECTED;
}
