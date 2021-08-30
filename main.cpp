#include <spdlog/spdlog.h>
#include "tef/aurora/safety.h"

#define Sleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))


int main(int argc, char* argv[])
{
	spdlog::set_level(spdlog::level::debug);

	TEF::Aurora::SmartFuse smartFuse;

	smartFuse.Connect();


	int board = 7;
	int before, after;
	smartFuse.GetCurrent(board, before);
	smartFuse.SetFet(board, true, after);

	spdlog::info("difference: {}", after - before);

	//TEF::Aurora::Safety safety;

	//safety.LoadCurrentMatrix();

	//safety.FilterCurrentMatrix();

	//safety.PrintCurrentMatrix();

	return true;

	//currently segfaulint when trying to delete cmLoad;
}

/*
*
*
void waitForEnter()
{
	do {} while (std::cin.get() != '\n');
}
* #include <chrono>
#include "tef/aurora/effectRunner.h"
#include "tef/aurora/effects/debugEffect.h"
#include "tef/aurora/smartFuse.h"
#include <shared_mutex>
#include <atomic>
#include "tef/aurora/testSuite.h"
#include "tef/aurora/safety.h"
#include <iostream>
* 	TEF::Aurora::Safety safety;
	safety.BuildCurrentMatrix();
	safety.PrintCurrentMatrix();
*/
/*
{
	pPaw->r = 10;
	Sleep(sleep);
	smartFuse.GetCurrent(7, r);
	spdlog::debug("Current: {}", r);
	Sleep(5000);
	pPaw->Black();
}

{
	pPaw->g = 10;
	Sleep(sleep);
	smartFuse.GetCurrent(7, g);
	spdlog::debug("Current: {}", g);
	Sleep(5000);
	pPaw->Black();
}

{
	pPaw->b = 10;
	Sleep(sleep);
	smartFuse.GetCurrent(7, b);
	spdlog::debug("Current: {}", b);
	Sleep(5000);
	pPaw->Black();
}
*/

/*
for (int i = 0; i <= 7; i++)
{
	pPaw->r = i;
	pPaw->g = i;
	Sleep(sleep);
	smartFuse.GetCurrent(7, r);

	pPaw->Black();

	pPaw->g = i;
	pPaw->b = i;
	Sleep(sleep);
	smartFuse.GetCurrent(7, g);

	pPaw->Black();

	pPaw->b = i;
	pPaw->r = i;
	Sleep(sleep);
	smartFuse.GetCurrent(7, b);

	pPaw->Black();

	std::cout << i << "," << r << "," << g << "," << b << std::endl;
}
*/



/*
void ShovelDemo()
{
	std::atomic_bool running = true;
	TEF::Aurora::MasterController masterController;
	TEF::Aurora::EffectRunner effectRunner("localhost");

	auto pPaw = std::make_shared<TEF::Aurora::Effects::PawEffect>();

	auto pCrap = std::make_shared<TEF::Aurora::Effects::PawEffect>();

	effectRunner.AddEffect(pPaw);
	effectRunner.AddEffect(pCrap);

	masterController.GetUserControl()->RegisterBool("rainbow effect", [&pCrap](bool running) {
		pCrap->m_running = running;
		return true;
		});

	masterController.GetUserControl()->RegisterLimitedInt("set red to", [&pPaw](int r) {
		pPaw->r = r;
		return true;
		});

	masterController.GetUserControl()->RegisterLimitedInt("set green to", [&pPaw](int g) {
		pPaw->g = g;
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
*/
