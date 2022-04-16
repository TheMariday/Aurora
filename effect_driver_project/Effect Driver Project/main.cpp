#include "LedHarness.h"
#include <iostream>
#include <cmath>
#include "BasicEffects.h"
#include "Metronome.h"

#define timestamp std::chrono::system_clock::time_point
#define duration std::chrono::milliseconds
#define duration_cast std::chrono::duration_cast<duration>


Loc GetRandomLoc(Harness& harness, bool backOnly = false)
{
	std::vector<LED*> leds = harness.GetMap();
	int random = rand() % leds.size();
	Loc loc = leds[random]->loc;

	if (backOnly && loc.y < 0)
		return GetRandomLoc(harness, backOnly);
	else
		return leds[random]->loc;
}

int main()
{
	Harness led_harness;
	led_harness.LoadCalibration("D:\\Users\\Sam\\GIT\\Aurora\\scripts\\LEDMapper\\led_positions.csv");

	led_harness.RenderToScreen(); // this will wait for user to press a key

	timestamp now = std::chrono::system_clock::now();

	Metronome metronome(now, 88);

	Loc leftEye = { 76, -72 , 702 };

	/*
	LocalisedFire fire(leftEye);
	fire.AddDriver(
		[&fire, &metronome](timestamp t) {
			Cycle<int>(&fire.orbMask.center.x, t, -76, 76, fire.GetStartTime(), metronome.Beats(1), CycleType::SINE);
		});
	*/

	std::vector<std::shared_ptr<Effect>> effects; 


	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(4),	metronome.Beats(1), 0.64f));
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(5),	metronome.Beats(1), 0.64f));
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(6),	metronome.Beats(1), 0.64f));
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(6.5),	metronome.Beats(1), 0.64f));
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(7),	metronome.Beats(1), 0.64f));
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(7.5),	metronome.Beats(1), 0.64f));
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(8),	metronome.Beats(1), 0.64f));
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(9),	metronome.Beats(1), 0.64f));
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(10),	metronome.Beats(1), 0.64f));
	while (true)
	{
		timestamp frame_time = std::chrono::system_clock::now();
		for (std::shared_ptr<Effect> e : effects)
		{
			e->Update(led_harness, frame_time);
		}

		if (led_harness.RenderToScreen(false, 0) == 27)
			break;

		led_harness.Black();
	}


	return 0;
}