#include "tef/aurora/button.h"
#include <spdlog/spdlog.h>
#include <sstream>
#include "tef/aurora/settings.h"

bool TEF::Aurora::Button::Connect(int pin) {
	std::stringstream ss;
	ss << "gpio export " << pin << " in";
	spdlog::debug("calling system {}", ss.str());
	system(ss.str().c_str());

	m_pin = pin;

	wiringPiSetup(); // This doesn't have a return code and just calls quit() if it fails and I hate it.

	pinMode(m_pin, INPUT);
	m_debounce = std::chrono::microseconds(Settings::TIME_DEBOUNCE);

	m_lastCallback = std::chrono::high_resolution_clock::now();

	SetFPS(Settings::FPS_STANDARD);

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

bool TEF::Aurora::DacButton::Connect(DacMCP3008* pDac, int pin, int debounceTime, int refreshRate) {

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

	SetFPS(refreshRate);

	return true;
}

bool TEF::Aurora::DacButton::IsConnected()
{
	return m_state != -1;
}

bool TEF::Aurora::DacButton::MainLoopCallback()
{
	voltage volts;
	m_pDac->Read(m_pin, volts);

	int state = VoltageToState(volts);

	std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();

	if ((t > m_lastCallback + m_debounce) and (state != m_state))
	{
		switch (state)
		{
		case -1:
			if (m_disconnectCallback) m_disconnectCallback();
			break;
		case 0:
			if (m_state == -1)
			{
				if (m_reconnectCallback) m_reconnectCallback();
			}
			else
			{
				if (m_upCallback) m_upCallback();
			}

			break;
		case 1:
			if (m_downCallback) m_downCallback();
			break;
		}
		m_state = state;

	}

	m_lastCallback = t;

	return true;
}

int TEF::Aurora::DacButton::VoltageToState(voltage volts)
{
	if (volts > 2.2)
		return 1;
	else if (volts > 1.1)
		return 0;
	else
		return -1;
}
