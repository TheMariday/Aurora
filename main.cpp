#include <spdlog/spdlog.h>
#include <chrono>
#include "tef/aurora/effectRunner.h"
#include "tef/aurora/effects/simpleEffect.h"
#include "tef/aurora/effects/paw.h"
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
	
	auto pPaw = std::make_shared<TEF::Aurora::Effects::PawEffect>();

	auto pCrap = std::make_shared<TEF::Aurora::Effects::PawEffect>();

	effectRunner.AddEffect(pPaw);
	effectRunner.AddEffect(pCrap);

	masterController.GetUserControl()->RegisterBool("rainbow effect", [&pCrap](bool b) {
		pCrap->m_running = b;
		return true;
		});

	masterController.GetUserControl()->RegisterLimitedInt("set red to", [&pPaw](int r) {
		pPaw->g = r;
		return true;
		});

	masterController.GetUserControl()->RegisterLimitedInt("set green to", [&pPaw](int g) {
		pPaw->r = g;
		return true;
		});

	masterController.GetUserControl()->RegisterLimitedInt("set blue to", [&pPaw](int b) {
		pPaw->b = b;
		return true;
		});

	masterController.GetUserControl()->RegisterLimitedInt("set all to", [&pPaw](int i) {
		pPaw->r = i;
		pPaw->g = i;
		pPaw->b = i;
		return true;
		});

	effectRunner.Run();
	masterController.Start();

	while (running)
		Sleep(1000);

	return true;
}
/*
auto pRedEffect = std::make_shared<TEF::Aurora::Effects::RedEffect>();
	effectRunner.AddEffect(pRedEffect);

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

	masterController.GetUserControl()->RegisterVoid("get simple effect brightness", [&pRedEffect, &masterController]() {
		std::stringstream ss;
		ss << "simple effect brightness is set to " << pRedEffect->m_brightness;
		masterController.GetSound()->AddSpeech(ss);
		return true;
		}, false);

	masterController.GetUserControl()->RegisterVoid("quit", [&running]() { running = false; return true; });
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