#include "tef/aurora/effect.h"
#include <chrono>
#include <thread>

namespace TEF::Aurora::Effects
{
	class SimpleEffect : public Effect{
	public:
		SimpleEffect()
		{
			SetFPS(1);
			printf("simple effect created 2\n");
		};

		bool MainLoopCallback() override 
		{
			printf("simple main loop looping 2\n");
			std::this_thread::sleep_for(std::chrono::microseconds(1000));
			return true;
		}
	};
};
