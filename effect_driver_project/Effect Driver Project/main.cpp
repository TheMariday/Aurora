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

	auto fireTexture = std::make_shared<SolidTexture>(&pose_default, RED);
	fireTexture->AddDriver([fireTexture, &tap](timestamp t) {
		Cycle<float>(&fireTexture->m_hsv.h, t, 0.0f, 0.15f, tap.Beat(0), tap.Beats(0), CycleType::RANDOM);
		});

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
					Ease<int>(&handwashMask->m_center, t, rightHand.x, leftHand.x, tap.Beat(19), tap.Beat(22), EaseType::BEIZIER);
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
				auto orbEffect = std::make_shared<Effect>(&pose_t, tap.Beat(24), tap.Beat(30));
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
			auto heartEffect = std::make_shared<Effect>(&pose_default, tap.Beat(28), tap.Beat(32));
			auto heartMask = std::make_shared<GroupMask>(&pose_default, "heart");
			heartMask->AddDriver([heartMask, &tap](timestamp t) {
				Ease<float>(&heartMask->m_intensity, t, 0.0f, 1.0f, tap.Beat(28), tap.Beat(30));
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

		//	34	one match that i can make an explosion
		{
			{ // fire hand orb
				auto orbEffect = std::make_shared<Effect>(&pose_t, tap.Beat(32), tap.Beat(40));
				Loc rh = pose_t.GetMarker("marker_right_hand");
				Loc lh = pose_t.GetMarker("marker_left_hand");
				auto orbMask = std::make_shared<OrbMask>(&pose_t, rh, 300);

				orbEffect->SetMask(orbMask);
				orbEffect->SetTexture(fireTexture);

				effects.push_back(orbEffect);

				orbMask->AddDriver([orbMask, &rh, &lh, &tap](timestamp t) {
					Ease<int>(&orbMask->m_center.x, t, rh.x, lh.x, tap.Beat(35.5), tap.Beat(36));
					});
			}
		}

		//	36	-plosion and all the

		{
			auto fireballEffect = std::make_shared<Effect>(&pose_t, tap.Beat(36), tap.Beat(40));
			auto fadeMask = std::make_shared< GlowMask>(&pose_t, pose_t.GetMarker("marker_left_hand"), 1000);

			fadeMask->AddDriver([fadeMask, &tap](timestamp t) {
				Ease<int>(&fadeMask->m_maxDistance, t, 1000, 0, tap.Beat(37), tap.Beat(39));
				}
			);

			fireballEffect->SetMask(fadeMask);

			fireballEffect->SetTexture(fireTexture);

			effects.push_back(fireballEffect);
		}

		{ // fire hand orb
			auto orbEffect = std::make_shared<Effect>(&pose_t, tap.Beat(40.5), tap.Beat(41));
			orbEffect->SetMask(std::make_shared<OrbMask>(&pose_t, pose_t.GetMarker("marker_right_hand"), 300));
			orbEffect->SetTexture(fireTexture);
			effects.push_back(orbEffect);
		}

		//	40	things i didn't say

		{
			Loc leftHip = { 97, -12, -261 };
			Loc rightHip = { -97, -12, -261 };
			Loc leftFoot = pose_t.GetMarker("marker_left_foot");
			Loc rightFoot = pose_t.GetMarker("marker_right_foot");

			{ // fire hand orb left
				auto orbEffect = std::make_shared<Effect>(&pose_t, tap.Beat(40), tap.Beat(40.5));
				orbEffect->SetTexture(fireTexture);

				auto orbMask = std::make_shared<OrbMask>(&pose_t, leftHip, 300);

				orbMask->AddDriver([orbMask, &leftHip, &leftFoot, &tap](timestamp t) {
					LocEase(&orbMask->m_center, t, leftHip, leftFoot, tap.Beat(40), tap.Beat(40.5));
					});

				orbEffect->SetMask(orbMask);

				effects.push_back(orbEffect);
			}

			{ // fire hand orb right
				auto orbEffect = std::make_shared<Effect>(&pose_t, tap.Beat(41), tap.Beat(41.5));
				orbEffect->SetTexture(fireTexture);

				auto orbMask = std::make_shared<OrbMask>(&pose_t, leftHip, 300);

				orbMask->AddDriver([orbMask, &rightHip, &rightFoot, &tap](timestamp t) {
					LocEase(&orbMask->m_center, t, rightHip, rightFoot, tap.Beat(41), tap.Beat(41.5));
					});

				orbEffect->SetMask(orbMask);

				effects.push_back(orbEffect);
			}
		}

		//	42 ish	were wrecking balls
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
		{
			Loc rh = pose_t.GetMarker("marker_right_hand");
			Loc lh = pose_t.GetMarker("marker_left_hand");

			{ // white left hand orb
				auto orbEffect = std::make_shared<Effect>(&pose_t, tap.Beat(42), tap.Beat(54));
				orbEffect->SetTexture(std::make_shared<SolidTexture>(&pose_default, WHITE));
				auto orbMask = std::make_shared<OrbMask>(&pose_t, lh);
				orbMask->AddDriver([orbMask, &tap](timestamp t) {
					Ease<int>(&orbMask->m_diameter, t, 300, 1500, tap.Beat(42), tap.Beat(52));
					Ease<int>(&orbMask->m_diameter, t, 1500, 0, tap.Beat(52), tap.Beat(54));
					});
				orbEffect->SetMask(orbMask);
				effects.push_back(orbEffect);
			}

			{ // white right hand orb
				auto orbEffect = std::make_shared<Effect>(&pose_t, tap.Beat(43), tap.Beat(54));
				orbEffect->SetTexture(std::make_shared<SolidTexture>(&pose_default, WHITE));
				auto orbMask = std::make_shared<OrbMask>(&pose_t, rh);
				orbMask->AddDriver([orbMask, &tap](timestamp t) {
					Ease<int>(&orbMask->m_diameter, t, 300, 1500, tap.Beat(43), tap.Beat(52));
					Ease<int>(&orbMask->m_diameter, t, 1500, 0, tap.Beat(52), tap.Beat(54));
					});
				orbEffect->SetMask(orbMask);
				effects.push_back(orbEffect);
			}

			// distortion time!
			{
				auto distortEffect = std::make_shared<Effect>(&pose_default, tap.Beat(42), tap.Beat(52));
				auto blackTexture = std::make_shared<SolidTexture>(&pose_default, BLACK);
				distortEffect->SetTexture(blackTexture);

				auto randomMask = std::make_shared<RandomMask>(&pose_default);
				randomMask->AddDriver([randomMask, &tap](timestamp t) {
					Ease<float>(&randomMask->m_prob, t, 0.0f, 0.5f, tap.Beat(42), tap.Beat(52));
					}
				);
				distortEffect->SetMask(randomMask);

				effects.push_back(distortEffect);
			}
		}
		//glowing eyes bit, to be replaced with actual eyes
		{
			auto eyeGlowEffect = std::make_shared<Effect>(&pose_default, tap.Beat(52), tap.Beat(56));
			eyeGlowEffect->SetTexture(std::make_shared<SolidTexture>(&pose_default, WHITE));
			eyeGlowEffect->SetMask(std::make_shared<GroupMask>(&pose_default, "eyes"));
			effects.push_back(eyeGlowEffect);
		}

		//56 punch fight
		{
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(56), tap.Beats(1), pose_t.GetMarker("marker_right_hand"), BLUE, 2000, 100, false));
		}

		//56.5 punch song
		{
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(56.5), tap.Beats(1), pose_t.GetMarker("marker_left_hand"), BLUE, 2000, 100, false));

		}

		//57 wipe back
		{
			auto leftArmFill = std::make_shared<Effect>(&pose_t, tap.Beat(57), tap.Beat(57.5));

			leftArmFill->SetTexture(std::make_shared<SolidTexture>(&pose_t, BLUE));

			leftArmFill->SetMask(std::make_shared<GroupMask>(&pose_t, "left_arm"));

			effects.push_back(leftArmFill);

		}

		//58 take
		//59 back my

		//60 life
		{
			auto rightArmFill = std::make_shared<Effect>(&pose_t, tap.Beat(60), tap.Beat(61));
			rightArmFill->SetTexture(std::make_shared<SolidTexture>(&pose_t, BLUE));
			rightArmFill->SetMask(std::make_shared<GroupMask>(&pose_t, "right_arm"));
			effects.push_back(rightArmFill);

			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(60), tap.Beats(4), pose_t.GetMarker("marker_right_hand"), WHITE, 2000, 100, true));
		}
		//60.5 left
		{
			auto leftArmFill = std::make_shared<Effect>(&pose_t, tap.Beat(60.5), tap.Beat(61));
			leftArmFill->SetTexture(std::make_shared<SolidTexture>(&pose_t, BLUE));
			leftArmFill->SetMask(std::make_shared<GroupMask>(&pose_t, "left_arm"));
			effects.push_back(leftArmFill);
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(60.5), tap.Beats(4), pose_t.GetMarker("marker_left_hand"), WHITE, 2000, 100, true));
		}
		//61 song
		{}
		// 62 prove

		// 63 im all

		//64 right strike

		{
			auto handWash = std::make_shared<Effect>(&pose_t, tap.Beat(64), tap.Beat(65));
			auto handwashMask = std::make_shared<BandMask>(&pose_t, 0, x_axis, 100);
			Loc hand = pose_t.GetMarker("marker_right_hand");
			handwashMask->AddDriver([handwashMask, hand, &tap](timestamp t) {
				Ease<int>(&handwashMask->m_center, t, 0, hand.x, tap.Beat(64), tap.Beat(65));
				}
			);
			handWash->SetTexture(blueTexture);
			handWash->SetMask(handwashMask);

			effects.push_back(handWash);
		}

		//64.5 left strike 

		{
			auto handWash = std::make_shared<Effect>(&pose_t, tap.Beat(64.5), tap.Beat(65.5));
			auto handwashMask = std::make_shared<BandMask>(&pose_t, 0, x_axis, 100);
			Loc hand = pose_t.GetMarker("marker_left_hand");
			handwashMask->AddDriver([handwashMask, hand, &tap](timestamp t) {
				Ease<int>(&handwashMask->m_center, t, 0, hand.x, tap.Beat(64.5), tap.Beat(65.5));
				}
			);
			handWash->SetTexture(blueTexture);
			handWash->SetMask(handwashMask);

			effects.push_back(handWash);
		}

		//65 wipe sooooo
		{
			auto radialBlueSpin = std::make_shared<Effect>(&pose_t, tap.Beat(65), tap.Beat(70));
			auto radialMask = std::make_shared<RadialMask>(&pose_t);
			radialBlueSpin->SetTexture(std::make_shared<SolidTexture>(&pose_t, BLUE));

			radialMask->AddDriver([radialMask, &tap](timestamp t) {
				Ease<float>(&radialMask->m_offset, t, 0.0f, 10.0f, tap.Beat(65), tap.Beat(70));
				});

			radialBlueSpin->SetMask(radialMask);
			effects.push_back(radialBlueSpin);
		}
		//68 end wipe ng

		//70 my 
		// 71 powers 

		// 72 turned #
		{
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(72), tap.Beats(1), pose_t.GetMarker("marker_right_hand"), BLUE, 1500, 200, true));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(72), tap.Beats(1), pose_t.GetMarker("marker_left_hand"), BLUE, 1500, 200, true));
		}


		// 73 on st

		{
			auto chestGlowEffect = std::make_shared<Effect>(&pose_t, tap.Beat(73), tap.Beat(76));
			auto fadeMask = std::make_shared< GlowMask>(&pose_t, pose_t.GetMarker("center"), 1000);

			fadeMask->AddDriver([fadeMask, &tap](timestamp t) {
				Ease<int>(&fadeMask->m_maxDistance, t, 1000, 0, tap.Beat(73), tap.Beat(76));
				}
			);

			chestGlowEffect->SetMask(fadeMask);

			chestGlowEffect->SetTexture(std::make_shared<SolidTexture>(&pose_t, BLUE));

			effects.push_back(chestGlowEffect);
		}

		// 74 arting right

		// 75 now ill

		// 76 be 
		{
			auto rightArmFill = std::make_shared<Effect>(&pose_t, tap.Beat(76), tap.Beat(78));
			rightArmFill->SetTexture(std::make_shared<SolidTexture>(&pose_t, WHITE));
			rightArmFill->SetMask(std::make_shared<GroupMask>(&pose_t, "right_arm"));
			effects.push_back(rightArmFill);
		}

		// 77 strong

		{
			auto leftArmFill = std::make_shared<Effect>(&pose_t, tap.Beat(77), tap.Beat(78));
			leftArmFill->SetTexture(std::make_shared<SolidTexture>(&pose_t, WHITE));
			leftArmFill->SetMask(std::make_shared<GroupMask>(&pose_t, "left_arm"));
			effects.push_back(leftArmFill);
		}

		// 78 ill

		// 79 play my

		// 80 fight
		{
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(80), tap.Beats(2), pose_t.GetMarker("marker_right_hand"), BLUE, 2000, 200, false));
			effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(80.5), tap.Beats(2), pose_t.GetMarker("marker_left_hand"), BLUE, 2000, 200, false));
		}

		// 81 song
		{
			auto leftArmFill = std::make_shared<Effect>(&pose_t, tap.Beat(81), tap.Beat(81.5));
			leftArmFill->SetTexture(std::make_shared<SolidTexture>(&pose_t, BLUE));
			leftArmFill->SetMask(std::make_shared<GroupMask>(&pose_t, "left_arm"));
			effects.push_back(leftArmFill);
		}

		// 83 inna

		// 84 don really

		{ // white hand orb
			auto orbEffect = std::make_shared<Effect>(&pose_t, tap.Beat(84), tap.Beat(84.5));
			orbEffect->SetMask(std::make_shared<OrbMask>(&pose_t, pose_t.GetMarker("marker_right_hand"), 300));
			orbEffect->SetTexture(std::make_shared<SolidTexture>(&pose_default, WHITE));

			effects.push_back(orbEffect);
		}

		// 85 care if

		// 86 no body

		{ // white hand orb
			auto orbEffect = std::make_shared<Effect>(&pose_t, tap.Beat(86), tap.Beat(86.5));
			orbEffect->SetMask(std::make_shared<OrbMask>(&pose_t, pose_t.GetMarker("marker_right_hand"), 300));
			orbEffect->SetTexture(std::make_shared<SolidTexture>(&pose_default, WHITE));

			effects.push_back(orbEffect);
		}

		// 87 else be

		// 88 liee // reach back

		{
			auto handWash = std::make_shared<Effect>(&pose_t, tap.Beat(88), tap.Beat(92));
			auto handwashMask = std::make_shared<BandMask>(&pose_t, 0, x_axis, 100);
			Loc rh = pose_t.GetMarker("marker_right_hand");

			handwashMask->AddDriver([handwashMask, rh, &tap](timestamp t) {
				Ease<int>(&handwashMask->m_center, t, 0, rh.x, tap.Beat(88), tap.Beat(90), EaseType::BEIZIER);
				Ease<int>(&handwashMask->m_center, t, rh.x, 0, tap.Beat(90), tap.Beat(92), EaseType::BEIZIER);

				}
			);

			handWash->SetMask(handwashMask);
			handWash->SetMask(std::make_shared<GroupMask>(&pose_t, "arms"));

			handWash->SetTexture(std::make_shared<LinearRainbowTexture>(&pose_t, pose_t.GetMarker("center"), x_axis));

			effects.push_back(handWash);
		}

		// 91 -vs cause

		// 92 i've still

		// 93  got

		// 94 allotta fight

		// 95 is my 

		// 96 fight (hit)

		// 96.5 (hit)

		effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(96), tap.Beats(1), pose_t.GetMarker("marker_right_hand"), RED, 2000, 200, false));
		effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(96.5), tap.Beats(1), pose_t.GetMarker("marker_left_hand"), RED, 2000, 200, false));


		// 97 song

		// 98 take

		// 99 back my

		// 100 life (hit)

		// 100.5 (hit)

		effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(100), tap.Beats(1), pose_t.GetMarker("marker_right_hand"), GREEN, 2000, 200, false));
		effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(100.5), tap.Beats(1), pose_t.GetMarker("marker_left_hand"), GREEN, 2000, 200, false));


		// 101 song

		// 102 prove

		// 103 im all

		// 104 right (hit)

		// 104.5 (hit)

		effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(104), tap.Beats(1), pose_t.GetMarker("marker_right_hand"), BLUE, 2000, 200, false));
		effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(104.5), tap.Beats(1), pose_t.GetMarker("marker_left_hand"), BLUE, 2000, 200, false));

		// 105 so

		// 108 ng (hit)

		// 108.5 (hit)

		// 109 (hit)

		// 109.5 (hit)

		// 110 my (hit)

		effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(108), tap.Beats(1), pose_t.GetMarker("center"), RED, 2000, 200, false));
		effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(108.5), tap.Beats(1), pose_t.GetMarker("center"), GREEN, 2000, 200, false));
		effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(109), tap.Beats(1), pose_t.GetMarker("center"), BLUE, 2000, 200, false));
		effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(109.5), tap.Beats(1), pose_t.GetMarker("center"), GREY, 2000, 200, false));
		effects.push_back(std::make_shared<Ripple>(&pose_t, tap.Beat(110), tap.Beats(1), pose_t.GetMarker("center"), WHITE, 2000, 200, false));


		// 111 powers

		// 112 turned (big drop)

		// 113 on

		// 114 starting right

		// 115 now ill

		// 116 be str

		// 117 ong

		// 118 ill

		// 119 play my

		// 120 fight

		// 121 song

		// 123 anna

		// 124 don't really

		// 125 care if

		// 126 nobody

		// 127 else be-

		// 128 -lieve-

		// 131 -s cause

		// 132 i've still

		// 133 got a

		// 134 lotta fight

		// 135 left in

		// 136 me

		// 139 no

		// 140 i've still got a lotta fight left in me


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
		long long milliseconds = dur.count();

		if (milliseconds)
			std::cout << "Estimated RPi fps: " << (1000 / (milliseconds * 3)) << std::endl;

		// remove dead effects
		effects.erase(std::remove_if(effects.begin(), effects.end(), [](const std::shared_ptr<Effect>& x) {return x->HasStopped(); }), effects.end());


		int beat = tap.GetBeat(frame_time);
		if (pose_default.RenderToScreen(false, beat) == 27)
			break;

		for (LED& led : buffer.leds)
		{
			led.hsv.v *= 0.9f;
			led.hsv.s *= 0.9f;
		}



		//buffer.Black();
	}


	return 0;
}