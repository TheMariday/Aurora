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