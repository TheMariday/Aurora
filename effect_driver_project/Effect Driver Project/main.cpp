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

	auto blueTexture = std::make_shared<SolidTexture>(&pose_t, BLUE);


	{
		//	0	
		{
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(0), tap.Beats(3), GetRandomLoc(&pose_t, true), BLUE));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(1), tap.Beats(3), GetRandomLoc(&pose_t, true), BLUE));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(2), tap.Beats(3), GetRandomLoc(&pose_t, true), BLUE));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(2.5), tap.Beats(3), GetRandomLoc(&pose_t, true), BLUE));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(3), tap.Beats(3), GetRandomLoc(&pose_t, true), BLUE));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(3.5), tap.Beats(3), GetRandomLoc(&pose_t, true), BLUE));
		}
		// 4
		{
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(4), tap.Beats(3), GetRandomLoc(&pose_t, true), BLUE));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(5), tap.Beats(3), GetRandomLoc(&pose_t, true), BLUE));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(6), tap.Beats(3), GetRandomLoc(&pose_t, true), BLUE));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(7), tap.Beats(3), GetRandomLoc(&pose_t, true), BLUE));
		}
		//	8	like a small boat
		{

			{ // blue hand orb
				auto orbEffect = std::make_shared<Effect>(&pose_t, tap.Beat(6), tap.Beat(13));
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
		//	12 on the oc-
		{
			auto twinkle = std::make_shared<Effect>(&pose_default, tap.Beat(11), tap.Beat(15));
			auto twinkleMask = std::make_shared<RandomMask>(&pose_default, 0.0f);
			twinkleMask->AddDriver([twinkleMask, &tap](timestamp t)
				{
					Ease<float>(&twinkleMask->m_prob, t, 0.0f, 0.1f, tap.Beat(11), tap.Beat(13));
					Ease<float>(&twinkleMask->m_prob, t, 0.1f, 0.0f, tap.Beat(13), tap.Beat(15));
				}
			);
			twinkle->SetMask(twinkleMask);
			twinkle->SetMask(std::make_shared<GroupMask>(&pose_default, "right_arm"));
			twinkle->SetTexture(blueTexture);

			effects.push_back(twinkle);
		}
		//	16	-an, sending big wav-
		{

			{
				auto handWash = std::make_shared<Effect>(&pose_t, tap.Beat(15), tap.Beat(17));
				auto handwashMask = std::make_shared<BandMask>(&pose_t, 0, x_axis, 100);
				Loc hand = pose_t.GetMarker("marker_right_hand");
				handwashMask->AddDriver([handwashMask, hand, &tap](timestamp t) {
					Ease<int>(&handwashMask->m_center, t, 0, hand.x, tap.Beat(15), tap.Beat(17));
					}
				);

				handWash->SetTexture(blueTexture);
				handWash->SetMask(std::make_shared<GroupMask>(&pose_t, "right_arm"));
				handWash->SetMask(handwashMask);

				effects.push_back(handWash);
			}

			{
				auto handWash = std::make_shared<Effect>(&pose_t, tap.Beat(15), tap.Beat(17));
				auto handwashMask = std::make_shared<BandMask>(&pose_t, 0, x_axis, 100);
				Loc hand = pose_t.GetMarker("marker_left_hand");
				handwashMask->AddDriver([handwashMask, hand, &tap](timestamp t) {
					Ease<int>(&handwashMask->m_center, t, 0, hand.x, tap.Beat(15), tap.Beat(17));
					}
				);

				handWash->SetTexture(blueTexture);
				handWash->SetMask(std::make_shared<GroupMask>(&pose_t, "left_arm"));
				handWash->SetMask(handwashMask);

				effects.push_back(handWash);
			}

		}
		// 20 -es into mot-
		{

			{
				auto handWash = std::make_shared<Effect>(&pose_t, tap.Beat(19), tap.Beat(22));
				auto handwashMask = std::make_shared<BandMask>(&pose_t, 0, x_axis, 100);
				Loc rightHand = pose_t.GetMarker("marker_right_hand");
				Loc leftHand = pose_t.GetMarker("marker_left_hand");
				handwashMask->AddDriver([handwashMask, rightHand, leftHand, &tap](timestamp t) {
					Ease<int>(&handwashMask->m_center, t, rightHand.x, leftHand.x, tap.Beat(19), tap.Beat(22));
					}
				);

				handWash->SetTexture(blueTexture);
				handWash->SetMask(std::make_shared<GroupMask>(&pose_t, "arms"));
				handWash->SetMask(handwashMask);

				effects.push_back(handWash);
			}

		}
		// 24 -ion, like how a single
		{
			{ // white hand orb
				auto orbEffect = std::make_shared<Effect>(&pose_t, tap.Beat(22), tap.Beat(30));
				auto orbMask = std::make_shared<OrbMask>(&pose_t, pose_t.GetMarker("marker_right_hand"), 300);

				orbMask->AddDriver([orbMask, &tap](timestamp t) {
					Ease<float>(&orbMask->m_intensity, t, 0.0f, 1.0f, tap.Beat(22), tap.Beat(24));
					Ease<float>(&orbMask->m_intensity, t, 1.0f, 0.0f, tap.Beat(24), tap.Beat(30));
					});

				orbEffect->SetMask(orbMask);
				orbEffect->SetTexture(std::make_shared<SolidTexture>(&pose_default, WHITE));

				effects.push_back(orbEffect);
			}
		}
		// 28 word can make a heart o-
		{
			auto heartEffect = std::make_shared<Effect>(&pose_default, tap.Beat(26), tap.Beat(32));
			auto heartMask = std::make_shared<GroupMask>(&pose_default, "heart");
			heartMask->AddDriver([heartMask, &tap](timestamp t) {
				Ease<float>(&heartMask->m_intensity, t, 0.0f, 1.0f, tap.Beat(26), tap.Beat(30));
				Ease<float>(&heartMask->m_intensity, t, 1.0f, 0.0f, tap.Beat(30), tap.Beat(34));
				}
			);
			heartEffect->SetTexture(std::make_shared<SolidTexture>(&pose_default, RED));
			heartEffect->SetMask(heartMask);

			effects.push_back(heartEffect);
		}
		// 30 pen, I might only have
		{
		}

		//	34	one match
		{
			{ // fire hand orb
				auto orbEffect = std::make_shared<Effect>(&pose_t, tap.Beat(32), tap.Beat(40));
				Loc rh = pose_t.GetMarker("marker_right_hand");
				Loc lh = pose_t.GetMarker("marker_left_hand");
				auto orbMask = std::make_shared<OrbMask>(&pose_t, rh, 300);

				orbEffect->SetMask(orbMask);
				auto fireTex = std::make_shared<SolidTexture>(&pose_default, RED);
				fireTex->AddDriver([fireTex, &tap](timestamp t) {
					Cycle<float>(&fireTex->m_hsv.h, t, 0.0f, 0.15f, tap.Beat(32), tap.Beats(0), CycleType::RANDOM);
					});
				orbEffect->SetTexture(fireTex);

				effects.push_back(orbEffect);

				orbMask->AddDriver([orbMask, &rh, &lh, &tap](timestamp t) {
					Ease<int>(&orbMask->m_center.x, t, rh.x, lh.x, tap.Beat(35.5), tap.Beat(36));
					});
			}
		}
		//		that I can
		//		make an ex-
		//	36	-plosion
		//		
		//		
		//		and all the
		//	40	things i
		//		didn't
		//		say
		//		were wrecking
		//	44	balls
		//		inside my
		//		brain
		//		I will
		//	48	scream
		//		em out
		//		loud tonight
		//		can you
		//	52	hear
		//		my voice
		//		this time
		//		this is my
		//	56	fight
		//		song
		//		
		//		take back my
		//	60	life
		//		song
		//		
		//		prove I'm all
		//	64	right
		//		song
		//		
		//		
		//	68	
		//		
		//		
		//		my powers
		//	72	turned
		//		on
		//		starting
		//		 now I'll
		//	76	be
		//		strong
		//		
		//		I'll play my
		//	80	fight
		//		song
		//		
		//		and I
		//	84	don't really
		//		care
		//		if no body 
		//		else be-
		//	88	-lieves
		//		
		//		
		//		cause
		//	92	I've
		//		still got a lot
		//		of fight
		//		this is my
		//	96	fight
		//		song
		//		
		//		take back my
		//	100	life
		//		song
		//		
		//		prove I'm all
		//	104	right
		//		song
		//		
		//		
		//	108	12345
		//		
		//		
		//		my powers
		//	112	turned
		//		on
		//		starting right
		//		now I'll
		//	116	be
		//		strong
		//		
		//		I'll play my
		//	120	fight
		//		song
		//		
		//		and I
		//	124	don't really
		//		care if
		//		if
		//		nobody else
		//	128	believes
		//		
		//		
		//		cause
		//	132	I've still
		//		
		//		got a lotta
		//		fight left in
		//	136	me
		//		
		//		
		//		
		//	140	no I've still
		//		got a lotta
		//		fight left
		//		in
		//	144	me
		{
		}

		effects.push_back(std::make_shared<BoostEffect>(&pose_default, tap.Beat(0), tap.Beat(144), &tap));
	}

	while (true)
	{
		timestamp frame_time = std::chrono::system_clock::now();

		auto start = std::chrono::high_resolution_clock::now();


		for (auto e : effects)
			e->Update(frame_time);


		auto stop = std::chrono::high_resolution_clock::now();

		duration dur = duration_cast(stop - start);
		int milliseconds = dur.count();

		std::cout << "fps: " << ((milliseconds == 0) ? -1 : (1000 / milliseconds)) << std::endl;

		// remove dead effects
		effects.erase(std::remove_if(effects.begin(), effects.end(), [](const std::shared_ptr<Effect>& x) {return x->HasStopped(); }), effects.end());


		int beat = tap.GetBeat(frame_time);
		if (pose_default.RenderToScreen(false, beat) == 27)
			break;

		buffer.Black();
	}


	return 0;
}