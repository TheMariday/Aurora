#include <assert.h>
#include <stdio.h>
#include <cstring>
#include <spdlog/spdlog.h>

#include "tef/aurora/effectRunner.h"
#include "tef/aurora/smartFuse.h"
#include "tef/aurora/properties.h"

#include "tef/aurora/effects/rainbowEffect.h"

int main(int argc, char** argv)
{
	spdlog::set_level(spdlog::level::debug);

	TEF::Aurora::MasterController mc;

	// if there are no args, then enable the cli
	bool cliEnabled = argc == 1;

	mc.Start(cliEnabled);
	mc.Spin();

	return 1;
}
