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
	mc.Start();
	mc.Spin();

	return 1;
}
