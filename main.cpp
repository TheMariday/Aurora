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
	fcsc.GetData();

}