#include <assert.h>
#include <stdio.h>
#include <string>
#include <spdlog/spdlog.h>

#include "tef/aurora/masterController.h"

int main()
{
	spdlog::set_level(spdlog::level::debug);

	TEF::Aurora::MasterController mc;

	mc.Start();

	std::this_thread::sleep_for(std::chrono::seconds(1000));

}