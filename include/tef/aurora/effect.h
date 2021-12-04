#pragma once
#include <vector>
#include <chrono>

#include "tef/aurora/led.h"

namespace TEF::Aurora {

	class Effect
	{
	public:
		Effect() = default;
		~Effect() = default;

		bool Start();
		bool Stop();

		bool IsRunning();

		virtual bool Render(std::vector<LED>& leds) = 0;

	protected:

		float SecondsSinceStart(); // returns time difference in seconds, returns 0 if not running

		std::chrono::high_resolution_clock::time_point m_startedAt;

	private:
		bool m_running = false;
	};

}