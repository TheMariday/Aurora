#pragma once
#include "tef/aurora/effect.h"
#include "atomic"

namespace TEF::Aurora::Effects
{

	class DebugEffect : public Effect
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

		void Black()
		{
			r = 0;
			g = 0;
			b = 0;
		}

		std::atomic_bool m_running = false;
		std::atomic_int r = 0;
		std::atomic_int g = 0;
		std::atomic_int b = 0;
	};

};
