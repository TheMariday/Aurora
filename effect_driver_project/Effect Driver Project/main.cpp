#include "LedHarness.h"
#include <iostream>
#include <cmath>
#include "Effects.h"
#include "Textures.h"
#include "Metronome.h"
#include "Texture.h"
#include "utils.h"
#include "colors.h"

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
	Harness pose_default = Harness(&buffer, "D:\\Users\\Sam\\GIT\\Aurora\\scripts\\LEDMapper\\led_positions.csv");
	Harness pose_left_hand = Harness(&buffer, "D:\\Users\\Sam\\GIT\\Aurora\\scripts\\LEDMapper\\led_positions_left_hand.csv");
	Harness pose_t = Harness(&buffer, "D:\\Users\\Sam\\GIT\\Aurora\\scripts\\LEDMapper\\led_positions_t_pose.csv");

	pose_left_hand.CopyGroups(&pose_default);
	pose_t.CopyGroups(&pose_default);

	std::thread t(system, "\"\"D:\\Program Files\\VLC\\vlc.exe\" \"C:\\Users\\marti\\Desktop\\fight song_mixdown.mp3\"\"");
	t.detach();

	pose_default.RenderToScreen(); // this will wait for user to press a key

	timestamp song_start = std::chrono::system_clock::now();

	Metronome tap(song_start, 88.02171202f, 4);

	std::vector<std::shared_ptr<Effect>> effects;

	{
		//	0	
		{
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(0), tap.Beats(3), GetRandomLoc(&pose_t, true), BLUE));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(1), tap.Beats(3), GetRandomLoc(&pose_t, true), RED));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(2), tap.Beats(3), GetRandomLoc(&pose_t, true), BLUE));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(2.5), tap.Beats(3), GetRandomLoc(&pose_t, true), RED));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(3), tap.Beats(3), GetRandomLoc(&pose_t, true), BLUE));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(3.5), tap.Beats(3), GetRandomLoc(&pose_t, true), RED));
		}

		//	4	like a small boat
		{
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(4), tap.Beats(3), GetRandomLoc(&pose_t, true), BLUE));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(5), tap.Beats(3), GetRandomLoc(&pose_t, true), RED));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(6), tap.Beats(3), GetRandomLoc(&pose_t, true), BLUE));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(7), tap.Beats(3), GetRandomLoc(&pose_t, true), RED));

			{ // blue hand orb
				auto orbEffect = std::make_shared<Effect>(&pose_t, tap.Beat(6), tap.Beat(13));
				auto blueTexture = std::make_shared<SolidTexture>(&pose_t, BLUE);
				auto orbMask = std::make_shared<OrbMask>(&pose_t, pose_t.GetMarker("marker_right_hand"), 300);

				orbMask->AddDriver([orbMask, &tap](timestamp t) {
					Ease<float>(&orbMask->m_intensity, t, 0.0f, 1.0f, tap.Beat(6), tap.Beat(8), EaseType::BEIZIER);
					Ease<float>(&orbMask->m_intensity, t, 1.0f, 0.0f, tap.Beat(11), tap.Beat(13), EaseType::BEIZIER);
					});

				orbEffect->SetMask(orbMask);
				orbEffect->SetTexture(blueTexture);

				effects.push_back(orbEffect);
			}

		}
	}

	while (true)
	{
		timestamp frame_time = std::chrono::system_clock::now();

		for (auto e : effects)
			e->Update(frame_time);


		// remove dead effects
		effects.erase(std::remove_if(effects.begin(), effects.end(), [](const std::shared_ptr<Effect>& x) {return x->HasStopped(); }), effects.end());


		int beat = tap.GetBeat(frame_time);
		if (pose_default.RenderToScreen(false, beat) == 27)
			break;

		buffer.Black();
	}


	return 0;
}