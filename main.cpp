#include <spdlog/spdlog.h>
#include <chrono>
#include <atomic>
#include "tef/aurora/runnable.h"
#include "tef/aurora/testSuite.h"

#define Sleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

int main(int argc, char* argv[])
{
	spdlog::set_level(spdlog::level::debug);

	TEF::Aurora::TestSuite::AutoTest();


	//EffectRunner effectRunner;
	//RedEffect redEffect;

	//std::shared_ptr<Effect> redEffect(new RedEffect);

	//effectRunner.AddEffect(redEffect);
	//effectRunner.Run();


	Sleep(10000);
}