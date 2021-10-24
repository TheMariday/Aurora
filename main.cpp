#include <assert.h>
#include <stdio.h>
#include <string>
#include <spdlog/spdlog.h>
#include "tef/aurora/sound.h"

int main()
{
	spdlog::set_level(spdlog::level::debug);

	TEF::Aurora::Sound tailBass, headset;

	std::string testAudio = "/home/pi/eminem.wav";

	tailBass.Connect("sndrpihifiberry", "softvol", "SoftMaster");
	headset.Connect("Chroma", "sysdefault:CARD=Chroma", "PCM");

	tailBass.Run();
	headset.Run();

	tailBass.PlayAudio(testAudio);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	tailBass.StopAudio(testAudio);

	tailBass.SetVolume(0.1f);
	tailBass.PlayAudio(testAudio);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	tailBass.StopAudio(testAudio);	

	tailBass.SetVolume(0.7f);
	tailBass.PlayAudio(testAudio);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	tailBass.StopAudio(testAudio);

	headset.AddSpeech("default volume level", true);
	headset.SetVolume(0.1f);
	headset.AddSpeech("10% volume level", true);
	headset.SetVolume(0.7f);
	headset.AddSpeech("70% volume level", true);


	std::this_thread::sleep_for(std::chrono::hours(1));
}

/*
* TEF::Aurora::DacMCP3008 dac;
	dac.Connect();
	TEF::Aurora::BatteryMonitor batteryMonitor;
	batteryMonitor.Connect(&dac);
	batteryMonitor.Run();

*/