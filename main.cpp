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

void ShovelDemo()
{
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
}

int main(int argc, char* argv[])
{
	spdlog::set_level(spdlog::level::debug);

	TEF::Aurora::SmartFuse smartFuse;
	smartFuse.Connect();
	smartFuse.Run();

	bool state = true;
	float current;
	while (true)
	{
		smartFuse.SetFet(7, state);
		smartFuse.GetCurrent(7, current);
		spdlog::debug("fet 7 current: {}", current);
		state = !state;
		Sleep(1000);
	}
	

	return true;
}