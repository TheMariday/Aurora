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
		Button(int pin = -1, int debounceTime = 100, int refreshRate = 100);

		~Button() {};

		bool RegisterCallbackDown(std::function<bool()> callback) { m_downCallback = callback; };
		bool RegisterCallbackUp(std::function<bool()> callback) { m_upCallback = callback; };

	private:

		bool MainLoopCallback() override;

		static bool DefaultCBDown();
		static bool DefaultCBUp();

		int m_pin;
		bool m_state = 0;
		
		std::function<bool()> m_downCallback;
		std::function<bool()> m_upCallback;

		std::chrono::high_resolution_clock::time_point  m_lastCallback;
		std::chrono::microseconds m_debounce;
	};
}