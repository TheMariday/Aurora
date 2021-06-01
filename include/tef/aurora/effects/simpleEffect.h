#pragma once
#include "tef/aurora/effect.h"
#include <spdlog/spdlog.h>

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
			for (LED& led : leds)
			{
				switch (led.index % 3) {
				case(0): led.r = 1; break;
				case(1): led.g = 1; break;
				case(2): led.b = 1; break;
				}
			}

			return true;
		}
	};

};
