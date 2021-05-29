#pragma once
#include <vector>
#include "tef/aurora/led.h"

namespace TEF::Aurora {

	class Effect
	{
	public:
		Effect() {
			m_startedAt = std::chrono::high_resolution_clock::now();
		};
		~Effect() {};

		virtual bool Render(std::vector<LED>& leds) = 0;

	protected:
		std::chrono::high_resolution_clock::time_point m_startedAt;
	};

}