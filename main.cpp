#include <assert.h>
#include <stdio.h>
#include <string>
#include <spdlog/spdlog.h>
#include "tef/aurora/dacMCP3008.h"
#include "tef/aurora/button.h"
#include "iostream"

int main()
{
	spdlog::set_level(spdlog::level::debug);


	TEF::Aurora::DacMCP3008 dac;
	TEF::Aurora::DacButton button;
	button.Connect(&dac, 6);

	button.Run();

	std::this_thread::sleep_for(std::chrono::hours(1));
}

/*
* TEF::Aurora::DacMCP3008 dac;
	dac.Connect();
	TEF::Aurora::BatteryMonitor batteryMonitor;
	batteryMonitor.Connect(&dac);
	batteryMonitor.Run();

*/