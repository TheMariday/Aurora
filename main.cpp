#include <spdlog/spdlog.h>
#include <chrono>
#include "tef/aurora/effectRunner.h"
#include "tef/aurora/effects/simpleEffect.h"
#include <shared_mutex>

#define Sleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

int main(int argc, char* argv[])
{
	spdlog::set_level(spdlog::level::debug);

	TEF::Aurora::EffectRunner effectRunner("localhost");


	auto pRedEffect = std::make_shared<TEF::Aurora::Effects::RedEffect>();

	effectRunner.AddEffect(pRedEffect);

	effectRunner.Run();

	Sleep(10000);
}