#include "LedHarness.h"
#include <iostream>
#include <cmath>
#include "BasicEffects.h"
#include "Metronome.h"

#define timestamp std::chrono::system_clock::time_point
#define duration std::chrono::milliseconds
#define duration_cast std::chrono::duration_cast<duration>


Loc GetRandomLoc(Harness* harness, bool backOnly = false)
{
	std::vector<LED*> leds = harness->GetGroup("main");
	int random = rand() % leds.size();
	Loc loc = harness->GetLoc(leds[random]);

	if (backOnly && loc.y < 0)
		return GetRandomLoc(harness, backOnly);
	else
		return loc;
}

int main()
{
	LedBuffer buffer;
	Harness pose_default =		Harness(&buffer, "D:\\Users\\Sam\\GIT\\Aurora\\scripts\\LEDMapper\\led_positions.csv");
	Harness pose_left_hand =	Harness(&buffer, "D:\\Users\\Sam\\GIT\\Aurora\\scripts\\LEDMapper\\led_positions_left_hand.csv");
	Harness pose_t =			Harness(&buffer, "D:\\Users\\Sam\\GIT\\Aurora\\scripts\\LEDMapper\\led_positions_t_pose.csv");

	pose_default.RenderToScreen(); // this will wait for user to press a key

	timestamp now = std::chrono::system_clock::now();

	Metronome metronome(now, 88);

	Loc leftEye = pose_default.GetMarker("eye_left");


	/*
	LocalisedFire fire(leftEye);
	fire.AddDriver(
		[&fire, &metronome](timestamp t) {
			Cycle<int>(&fire.orbMask.center.x, t, -76, 76, fire.GetStartTime(), metronome.Beats(1), CycleType::SINE);
		});
	*/

	std::vector<std::shared_ptr<Effect>> effects; 

	effects.push_back(std::make_shared<Ripple>(&pose_left_hand, metronome.Beat(0), metronome.Beats(4), pose_left_hand.GetMarker("marker_left_hand"), HSV( 1.0f, 0.0f, 1.0f ), 4000, 100, false));
	/*
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(4),	metronome.Beats(1), 0.64f));
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(5),	metronome.Beats(1), 0.64f));
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(6),	metronome.Beats(1), 0.64f));
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(6.5),	metronome.Beats(1), 0.64f));
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(7),	metronome.Beats(1), 0.64f));
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(7.5),	metronome.Beats(1), 0.64f));
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(8),	metronome.Beats(1), 0.64f));
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(9),	metronome.Beats(1), 0.64f));
	effects.push_back(std::make_shared<Ripple>(GetRandomLoc(led_harness, true), metronome.Beat(10),	metronome.Beats(1), 0.64f));
	*/
	while (true)
	{
		timestamp frame_time = std::chrono::system_clock::now();
		for (std::shared_ptr<Effect> e : effects)
		{
			e->Update(frame_time);
		}

		if (pose_default.RenderToScreen(false, 0) == 27)
			break;

		buffer.Black();
	}


	return 0;
}