#pragma once
#pragma once
#include "tef/aurora/effect.h"
#include <spdlog/spdlog.h>
#include "atomic"
#include "tef/aurora/masterController.h"

namespace TEF::Aurora::Effects
{

	class PawEffect : public Effect
	{
	public:

		bool Render(std::vector<TEF::Aurora::LED>& leds) override
		{

			for (LED& led : leds)
			{
				led.r = r / 10.0;
				led.g = g / 10.0;
				led.b = b / 10.0;
			}

			return true;
		}

		std::atomic_bool m_running = false;
		std::atomic_int r = 0;
		std::atomic_int g = 0;
		std::atomic_int b = 0;
	};

};
