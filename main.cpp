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

	spdlog::info("everything set up, sleeping");
	std::this_thread::sleep_for(std::chrono::seconds(5));
	
	int current;
	unsigned char devChannel = 3;

	
	smartFuse.GetCurrent(devChannel, current);
	spdlog::info("Current before enabling: {}", current);


	spdlog::warn("enabling channel");
	smartFuse.SetFet(devChannel, true, current);
	spdlog::info("Current after enabling: {}", current);

	spdlog::warn("starting effect");
	rainbowEffect->Start();
	std::this_thread::sleep_for(std::chrono::seconds(1));
	smartFuse.GetCurrent(devChannel, current);
	spdlog::info("Current after starting: {}", current);

	std::this_thread::sleep_for(std::chrono::seconds(5));

	spdlog::warn("stopping effect");
	rainbowEffect->Stop();
	std::this_thread::sleep_for(std::chrono::seconds(1));
	smartFuse.GetCurrent(devChannel, current);
	spdlog::info("Current after stopping: {}", current);

	spdlog::warn("disabling channel");
	smartFuse.SetFet(devChannel, false, current);
	spdlog::info("Current after disabling: {}", current);

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