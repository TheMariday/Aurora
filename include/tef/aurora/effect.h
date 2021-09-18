#pragma once
#include <vector>
#include "tef/aurora/led.h"
#include <chrono>

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

		int TimeSinceStarted(); // returns time difference in milis, returns 0 if not running

		std::chrono::high_resolution_clock::time_point m_startedAt;

	private:
		bool m_running = false;
	};

}