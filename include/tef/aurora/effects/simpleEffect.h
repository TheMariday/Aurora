#include "tef/aurora/effect.h"
#include <chrono>
#include <thread>

namespace TEF::Aurora::Effects
{
	class SimpleEffect : public Effect{
	public:
		bool MainLoopCallback() override 
		{
			printf("simple main loop looping\n");
			return true;
		}
	};
};
