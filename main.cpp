#include <assert.h>
#include <stdio.h>
#include <string>
#include <spdlog/spdlog.h>
#include "tef/aurora/fadecandyServerComms.h"

int main()
{
	spdlog::set_level(spdlog::level::debug);
	TEF::Aurora::FadecandyServerComms fcsc;
	fcsc.Connect();
	fcsc.StateChangeCallback([](std::string serial, bool connected) {spdlog::info("Fadecandy board {} has been {}", serial, connected ? "connected" : "disconnected"); });
	
	fcsc.Run();

	std::this_thread::sleep_for(std::chrono::hours(1));
}