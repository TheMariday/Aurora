#include "LedHarness.h"
#include <iostream>
#include <cmath>
#include "ExampleEffects.h"
#include "Metronome.h"

#define timestamp std::chrono::system_clock::time_point
#define duration std::chrono::milliseconds
#define duration_cast std::chrono::duration_cast<duration>

int main()
{
	Harness led_harness;
	led_harness.LoadCalibration("D:\\Users\\Sam\\GIT\\Aurora\\scripts\\LEDMapper\\led_positions.csv");

	led_harness.RenderToScreen(); // this will wait for user to press a key

	timestamp now = std::chrono::system_clock::now();

	Metronome metronome(now, 30);

	SolidColour solidColour(now);

	
	solidColour.AddDriver(
		[&solidColour](timestamp t) {
			Cycle<float>(&solidColour.hsv.h, t, 0, 0.12, solidColour.GetStartTime(), std::chrono::milliseconds(5000), CycleType::RANDOM);
			Cycle<float>(&solidColour.hsv.v, t, 0.8, 1, solidColour.GetStartTime(), std::chrono::milliseconds(5000), CycleType::RANDOM);
		});
		
	

	while (true)
	{
		timestamp frame_time = std::chrono::system_clock::now();
		solidColour.Update(led_harness, frame_time);

		if (led_harness.RenderToScreen(false, 0) == 27)
			break;
	}


	return 0;
}