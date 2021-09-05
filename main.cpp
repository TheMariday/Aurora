#include <assert.h>
#include <stdio.h>
#include <string>
#include <spdlog/spdlog.h>
#include "tef/aurora/batteryMonitor.h"

int main()
{
	spdlog::set_level(spdlog::level::debug);

	TEF::Aurora::DacMCP3008 dac;
	dac.Connect();
	TEF::Aurora::BatteryMonitor batteryMonitor;
	batteryMonitor.Connect(&dac);
	batteryMonitor.Run();

	std::this_thread::sleep_for(std::chrono::hours(1));
}