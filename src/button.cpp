#include "tef/aurora/button.h"
#include <spdlog/spdlog.h>
#include <sstream>

TEF::Aurora::Button::Button(int pin, int debounceTime, int refreshRate) :m_pin(pin) {
	std::stringstream ss;
	ss << "gpio export " << 2 << " in";
	spdlog::debug("calling system {}", ss.str());
	system(ss.str().c_str());

	wiringPiSetup(); // This doesn't have a return code and just calls quit() if it fails and I hate it.

	pinMode(m_pin, INPUT);
	m_debounce = std::chrono::microseconds(debounceTime);

	m_lastCallback = std::chrono::high_resolution_clock::now();

	SetFPS(refreshRate);

	m_downCallback = []() {spdlog::debug("unhandled down callback"); return true; };
	m_upCallback = []() {spdlog::debug("unhandled up callback"); return true; };
}

bool TEF::Aurora::Button::MainLoopCallback()
{
	int state = digitalRead(m_pin);

	std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();

	if ((t > m_lastCallback + m_debounce) and (state != m_state))
	{
		m_state = state;
		m_state ? m_downCallback() : m_upCallback();
	}

	m_lastCallback = t;

	return true;
}

TEF::Aurora::DacButton::DacButton(DacMCP3008& dac, int pin, int debounceTime, int refreshRate) {

	if (pin < 5)
	{
		spdlog::error("DacButton cannot connect to pin {} as pin is not set up for button usage", pin);
		return;
	}

	if (pin > 7)
	{
		spdlog::error("DacButton cannot connect to pin {} as pin is not set up for button usage", pin);
		return;
	}

	if (!dac.isConnected())
	{
		spdlog::warn("DacButton cannot connect to dac as dac is not connected, connecting dac");
		if (!dac.Connect())
		{
			spdlog::error("DacButton failed to connect to unconnected dac");
			return;
		}
	}

	m_pin = pin;
	m_pDac = &dac;

	m_debounce = std::chrono::microseconds(debounceTime);
	m_lastCallback = std::chrono::high_resolution_clock::now();

	SetFPS(refreshRate);

	m_downCallback = []() {spdlog::debug("unhandled down callback"); return true; };
	m_upCallback = []() {spdlog::debug("unhandled up callback"); return true; };
	m_disconnectCallback = []() {spdlog::debug("unhandled disconnect callback"); return true; };
	m_reconnectCallback = []() {spdlog::debug("unhandled reconnect callback"); return true; };
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
			m_disconnectCallback();
			break;
		case 0:
			if (m_state == -1)
				m_reconnectCallback();
			else
				m_upCallback();
			break;
		case 1:
			m_downCallback();
			break;
		}
		m_state = state;

	}

	m_lastCallback = t;

	return true;
}

int TEF::Aurora::DacButton::VoltageToState(voltage volts)
{
	//Needs calibration options
	if (volts > 2.75)
		return 1;
	else if (volts > 1.92)
		return 0;
	else
		return -1;
}
