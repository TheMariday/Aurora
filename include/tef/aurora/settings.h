#pragma once
#include <string>

// this entire file needs to be moved to a toml kinda thing.
// it'll do for now

namespace TEF::Aurora::Settings {

	// devices
	inline std::string DEVICE_HEADSET = "sysdefault:CARD=Device";
	inline std::string DEVICE_TAIL    = "sysdefault:CARD=Headphones";
	inline std::string DEVICE_FUSE    = "/dev/ttyUSB0";

	// pins
	inline int PIN_RECORD = 6;
	inline int PIN_CONFIRM = 7;

	inline int PIN_CELL_0 = 0;
	inline int PIN_CELL_1 = 1;
	inline int PIN_CELL_2 = 2;
	inline int PIN_CELL_3 = 3;

	// voltage dividers
	// default 16.17 -> 3.3v divider
	inline int DIVIDER_OHM_1 = 39'000;
	inline int DIVIDER_OHM_2 = 10'000;

	inline int VOLTAGE_CELL_MIN = 3.2;

	// timing

	inline int TIME_DEBOUNCE = 100;

	inline int FPS_STANDARD = 100;
	inline int FPS_EFFECT = 60;
	inline int FPS_BATTERY = 1;

	// networking

	inline std::string FADECANDY        = "192.168.1.109:7890";
	inline std::string FADECANDY_SOCKET = "ws://192.168.1.109:7890";

}