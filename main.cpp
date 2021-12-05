#include <assert.h>
#include <stdio.h>
#include <string>
#include <spdlog/spdlog.h>

#include "tef/aurora/effectRunner.h"
#include "tef/aurora/smartFuse.h"
#include "tef/aurora/properties.h"

#include "tef/aurora/effects/rainbowEffect.h"



int main(int argc, char** argv)
{
	spdlog::set_level(spdlog::level::debug);


	TEF::Aurora::EffectRunner effectRunner;
	TEF::Aurora::SmartFuse smartFuse;

	if (!smartFuse.Connect("/dev/ttyUSB0"))
	{
		spdlog::error("Master Controller failed to start smart fuse as it cannot connect");
		return 0;
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
			return 0;
		}

		smartFuse.Calibrate(devChannel, currentZero.value(), currentScale.value());
	}

	smartFuse.SetFPS(2);
	smartFuse.Run();

	if (!effectRunner.Connect("localhost"))
	{
		spdlog::error("failed to connect to effect runner");
		return 0;
	}

	auto rainbowEffect = std::make_shared<TEF::Aurora::Effects::RainbowEffect>();

	effectRunner.AddEffect(rainbowEffect);

	effectRunner.SetFPS(60);
	effectRunner.Run();

	std::this_thread::sleep_for(std::chrono::seconds(1));

	{
		float current;

		smartFuse.GetCurrent(devChannel, current);
		spdlog::info("Current closed: {}", current);

		smartFuse.SetFet(devChannel, true);

		smartFuse.GetCurrent(devChannel, current);
		spdlog::info("Current open: {}", current);

		rainbowEffect->Start();

		std::this_thread::sleep_for(std::chrono::seconds(1));

		smartFuse.GetCurrent(devChannel, current);
		spdlog::info("Current started: {}", current);

		std::this_thread::sleep_for(std::chrono::seconds(5));

		rainbowEffect->Stop();

		std::this_thread::sleep_for(std::chrono::seconds(1));

		smartFuse.GetCurrent(devChannel, current);
		spdlog::info("Current stopped: {}", current);

		smartFuse.SetFet(devChannel, false);

		smartFuse.GetCurrent(devChannel, current);
		spdlog::info("Current closed: {}", current);

	}

	/*
	TEF::Aurora::SmartFuse sf;

	spdlog::info("Master Controller starting smart fuse systems");
	if (!sf.Connect("/dev/ttyUSB0"))
	{
		spdlog::error("Master Controller failed to start smart fuse as it cannot connect");
		return 0;
	}

	sf.SetFPS(2);
	sf.Run();

	unsigned char channel = std::stoi(argv[1]);
	spdlog::info("Testing channel {}", channel);

	int c;

	sf.SetFet(channel, true, c);
	spdlog::info("Set high: {}", c);
	for (int i = 0; i < 5; ++i)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		sf.GetCurrent(channel, c);
		spdlog::info("High: {}", c);
	}

	sf.SetFet(channel, false, c);
	spdlog::info("Set low: {}", c);
	for (int i = 0; i < 5; ++i)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		sf.GetCurrent(channel, c);
		spdlog::info("Low: {}", c);
	}

	*/
	/*TEF::Aurora::MasterController mc;

	if (!mc.Start())
		return 0;

	while(!mc.HasQuit())
		std::this_thread::sleep_for(std::chrono::seconds(1));

	mc.GetNotifier()->PlayAudio("/home/pi/media/cyclops/AI_engine_down.wav", true);

	*/
	return 1;
}