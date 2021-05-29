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
				led.r = 1;
			return true;
		}
	};

};
