#pragma once
#include "tef/aurora/effect.h"
#include <spdlog/spdlog.h>
#include "atomic"
#include "tef/aurora/masterController.h"

namespace TEF::Aurora::Effects
{

	class RedEffect : public Effect
	{
	public:
		RedEffect() {};
		~RedEffect() {};

		bool Render(std::vector<TEF::Aurora::LED>& leds) override
		{
			if (m_running)
			{
				for (LED& led : leds)
				{
					switch (led.index % 3) {
					case(0): led.r = m_brightness / 10.0; break;
					case(1): led.g = m_brightness / 10.0; break;
					case(2): led.b = m_brightness / 10.0; break;
					}
				}
			}

			return true;
		}

		std::atomic_bool m_running = false;
		std::atomic_int m_brightness = 10;
	};

};
