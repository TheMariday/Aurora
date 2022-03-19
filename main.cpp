#include <assert.h>
#include <stdio.h>
#include <cstring>
#include <spdlog/spdlog.h>

#include "tef/aurora/effectRunner.h"
#include "tef/aurora/smartFuse.h"
#include "tef/aurora/properties.h"

#include "tef/aurora/effects/rainbowEffect.h"

void fuseTest()
{
	TEF::Aurora::EffectRunner effectRunner;
	TEF::Aurora::SmartFuse smartFuse;

	if (!smartFuse.Connect("/dev/ttyUSB0"))
	{
		spdlog::error("Master Controller failed to start smart fuse as it cannot connect");
		return;
	}

	unsigned char devChannel = 3;

	{
		std::stringstream ss;
		ss << "subsystem_" << static_cast<int>(devChannel);
		std::optional<int> currentZero = TEF::Aurora::Properties::GetProperty<float>(ss.str(), "currentZero");
		std::optional<float> currentScale = TEF::Aurora::Properties::GetProperty<float>(ss.str(), "currentScale");

		if (!currentZero.has_value() || !currentScale.has_value())
		{
			spdlog::error("Cannot load currentZero or currentScale");
			return;
		}

		smartFuse.Calibrate(devChannel, currentZero.value(), currentScale.value());
	}

	smartFuse.SetFPS(2);
	smartFuse.Run();

	if (!effectRunner.Connect("localhost"))
	{
		spdlog::error("failed to connect to effect runner");
		return;
	}

	auto rainbowEffect = std::make_shared<TEF::Aurora::Effects::RainbowEffect>();

	effectRunner.AddEffect("test", rainbowEffect);

	effectRunner.SetFPS(60);
	effectRunner.Run();

	std::this_thread::sleep_for(std::chrono::seconds(1));

	{
		float current;

		smartFuse.GetCurrent(devChannel, current);
		spdlog::info("Current closed: {0:.3f}A", current);

		smartFuse.SetFet(devChannel, true);

		smartFuse.GetCurrent(devChannel, current);
		spdlog::info("Current open: {0:.3f}A", current);

		rainbowEffect->Start();

		std::this_thread::sleep_for(std::chrono::seconds(1));

		smartFuse.GetCurrent(devChannel, current);
		spdlog::info("Current started: {0:.3f}A", current);

		std::this_thread::sleep_for(std::chrono::seconds(5));

		rainbowEffect->Stop();

		std::this_thread::sleep_for(std::chrono::seconds(1));

		smartFuse.GetCurrent(devChannel, current);
		spdlog::info("Current stopped: {0:.3f}A", current);

		smartFuse.SetFet(devChannel, false);

		smartFuse.GetCurrent(devChannel, current);
		spdlog::info("Current closed: {0:.3f}A", current);

	}
}


int main(int argc, char** argv)
{
	spdlog::set_level(spdlog::level::debug);

	TEF::Aurora::MasterController mc;
	mc.Start();
	mc.Spin();

	return 1;
}
