#include <assert.h>
#include <stdio.h>
#include <string>
#include <spdlog/spdlog.h>

#include "tef/aurora/masterController.h"

int main()
{
	spdlog::set_level(spdlog::level::debug);

	TEF::Aurora::MasterController mc;

	if (!mc.Start())
		return 0;

	while(!mc.HasQuit())
		std::this_thread::sleep_for(std::chrono::seconds(1));

	mc.GetNotifier()->PlayAudio("/home/pi/media/cyclops/AI_engine_down.wav", true);

	return 1;
}