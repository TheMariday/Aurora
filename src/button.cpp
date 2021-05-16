#include "tef/aurora/button.h"

TEF::Aurora::Button::Button(int pin, int debounceTime, int refreshRate) :m_pin(pin) {
	std::stringstream s;
	s << "gpio export " << 2 << " in";
	spdlog::debug("calling system {}", s.str());
	system(s.str().c_str());
	wiringPiSetup(); // This doesn't have a return code and just calls quit() if it fails and I hate it.
	pinMode(m_pin, INPUT);
	m_debounce = std::chrono::microseconds(debounceTime);

	m_lastCallback = std::chrono::high_resolution_clock::now();

	SetFPS(refreshRate);

	m_downCallback = DefaultCBDown;
	m_upCallback = DefaultCBUp;
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

bool TEF::Aurora::Button::DefaultCBDown()
{
	spdlog::debug("unhandled down callback");
	return true;
}

bool TEF::Aurora::Button::DefaultCBUp()
{
	spdlog::debug("unhandled up callback");
	return true;
}
