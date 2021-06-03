#include <spdlog/spdlog.h>
#include <chrono>
#include "tef/aurora/effectRunner.h"
#include "tef/aurora/effects/simpleEffect.h"
#include "tef/aurora/smartFuse.h"
#include <shared_mutex>
#include <atomic>
#include "tef/aurora/testSuite.h"

#define Sleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

int main(int argc, char* argv[])
{
	spdlog::set_level(spdlog::level::debug);

	std::atomic_bool running = true;

	TEF::Aurora::MasterController masterController;

	TEF::Aurora::EffectRunner effectRunner("localhost");

	auto pRedEffect = std::make_shared<TEF::Aurora::Effects::RedEffect>();

	masterController.GetUserControl()->RegisterVoid("start simple effect", [&pRedEffect]() {
		pRedEffect->m_running = true;
		return true;
		});

	masterController.GetUserControl()->RegisterVoid("stop simple effect", [&pRedEffect]() {
		pRedEffect->m_running = false;
		return true;
		});

	masterController.GetUserControl()->RegisterLimitedInt("set simple effect brightness to", [&pRedEffect](int brightness) {
		pRedEffect->m_brightness = brightness;
		return true;
		});

	masterController.GetUserControl()->RegisterVoid("quit", [&running]() { running = false; return true; });



	effectRunner.AddEffect(pRedEffect);
	effectRunner.Run();

	masterController.Start();

	while (running)
		Sleep(1000);

	return true;
}
/*

	TEF::Aurora::EffectRunner effectRunner("localhost");

	TEF::Aurora::SmartFuse smartFuse;
	smartFuse.Connect();
	Sleep(1000);
	smartFuse.Run();

	smartFuse.SetFet(7, false);

	auto pRedEffect = std::make_shared<TEF::Aurora::Effects::RedEffect>();

	effectRunner.AddEffect(pRedEffect);

	effectRunner.Run();
*/