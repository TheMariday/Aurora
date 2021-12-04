#pragma once
#include "tef/aurora/effect.h"
#include <spdlog/spdlog.h>
#include "atomic"
#include "tef/aurora/masterController.h"

namespace TEF::Aurora::Effects
{

	class RainbowEffect : public Effect
	{
	public:

		bool Render(std::vector<TEF::Aurora::LED>& leds) override
		{
			for (LED& led : leds)
			{
				float hue_static = static_cast<float>(led.index) / static_cast<float>(m_rainbow_width);
				float hue_animated = SecondsSinceStart() / static_cast<float>(m_rainbow_speed);
				led.HSV(hue_static + hue_animated, 1.0f, static_cast<float>(m_brightness) / 10.0f);
			}

			return true;
		}

		std::atomic_int m_brightness = 10;
		std::atomic_int m_rainbow_width = 64;
		std::atomic_int m_rainbow_speed = 2; // seconds per cycle
	};

};
