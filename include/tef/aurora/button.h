#pragma once
#include <spdlog/spdlog.h>
#include <wiringPi.h>
#include <sstream>
#include <chrono>
#include <tef/aurora/effect.h>
#include <functional>

namespace TEF::Aurora {

	class Button : public Effect
	{
	public:
		Button(int pin, int debounceTime = 100, int refreshRate = 100) :m_pin(pin) {
			std::stringstream s;
			s << "gpio export " << 2 << " in";
			spdlog::debug("calling system {}", s.str());
			system(s.str().c_str());
			wiringPiSetup();
			pinMode(m_pin, INPUT);
			m_debounce = std::chrono::microseconds(debounceTime);

			m_lastCallback = std::chrono::high_resolution_clock::now();

			SetFPS(refreshRate);

			m_state = 0;

			m_downCallback = DefaultCBDown;
			m_upCallback = DefaultCBUp;
		};

		~Button() {};

		void Callback(bool state)
		{
			std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();

			if ((t > m_lastCallback + m_debounce) and (state != m_state))
			{
				m_state = state;
				if (m_state)
				{
					m_downCallback();
				}
				else
				{
					m_upCallback();
				}
			}

			m_lastCallback = t;

		};

		static bool DefaultCBDown() { spdlog::debug("unhandled down callback"); return true; }
		static bool DefaultCBUp() { spdlog::debug("unhandled up callback"); return true; }

		bool MainLoopCallback()
		{
			Callback(digitalRead(m_pin));
		};

		bool RegisterCallbackDown(std::function<bool()> callback) { m_downCallback = callback; };
		bool RegisterCallbackUp(std::function<bool()> callback) { m_upCallback = callback; };

	private:
		int m_pin;
		std::chrono::high_resolution_clock::time_point  m_lastCallback;
		std::chrono::microseconds m_debounce;
		bool m_state;
		std::function<bool()> m_downCallback;
		std::function<bool()> m_upCallback;

	};
}