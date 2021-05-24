#pragma once
#include <wiringPi.h>
#include <chrono>
#include <functional>

#include "tef/aurora/runnable.h"

namespace TEF::Aurora {

	class Button : public Runnable
	{
	public:
		Button(int pin = -1, int debounceTime = 100, int refreshRate = 100);

		~Button() {};

		bool RegisterCallbackDown(std::function<bool()> callback = {}) { m_downCallback = callback; };
		bool RegisterCallbackUp(std::function<bool()> callback = {}) { m_upCallback = callback; };

	private:

		bool MainLoopCallback() override;

		int m_pin;
		bool m_state = 0;
		
		std::function<bool()> m_downCallback;
		std::function<bool()> m_upCallback;

		std::chrono::high_resolution_clock::time_point  m_lastCallback;
		std::chrono::microseconds m_debounce;
	};
}