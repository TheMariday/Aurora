#pragma once
#include "Effects.h"
#include "tef/aurora/effectRunner.h"

void FightSong(TEF::Aurora::EffectRunner* er, Harness* harness, timestamp song_start)
{
	std::shared_ptr<Metronome> tap = std::make_shared< Metronome>(song_start, 88.02171202f, 4);

	auto blueTexture = std::make_shared<SolidTexture>(harness, BLUE);

	auto fireTexture = std::make_shared<SolidTexture>(harness, RED);
	fireTexture->AddDriver([fireTexture, tap](timestamp t) {
		Cycle<float>(&fireTexture->m_hsv.h, t, 0.0f, 0.15f, tap->Beat(0), tap->Beats(0), CycleType::RANDOM);
		});


	/*
	eyes_default
	eyes_angry
	eyes_heart
	eyes_chevron
	eyes_cross
	eyes_lids
	eyes_big
	eyes_rings
	*/

	er->AddEffect(std::make_shared< GroupSolid>(harness, tap->Beat(0), tap->Beat(150), "eyes", BLACK)); // overrides the default eye effects

	er->AddEffect(std::make_shared< GroupSolid>(harness, tap->Beat(48), tap->Beat(56), "eyes_default"));
	er->AddEffect(std::make_shared< GroupSolid>(harness, tap->Beat(56), tap->Beat(60), "eyes_angry"));
	er->AddEffect(std::make_shared< GroupSolid>(harness, tap->Beat(60), tap->Beat(64), "eyes_default"));
	er->AddEffect(std::make_shared< GroupSolid>(harness, tap->Beat(64), tap->Beat(72), "eyes_v"));
	er->AddEffect(std::make_shared< GroupSolid>(harness, tap->Beat(72), tap->Beat(77), "eyes_angry"));
	er->AddEffect(std::make_shared< GroupSolid>(harness, tap->Beat(72), tap->Beat(80), "eyes_chevron"));
	er->AddEffect(std::make_shared< GroupSolid>(harness, tap->Beat(80), tap->Beat(84), "eyes_angry"));
	er->AddEffect(std::make_shared< GroupSolid>(harness, tap->Beat(84), tap->Beat(96), "eyes_default"));
	er->AddEffect(std::make_shared< GroupSolid>(harness, tap->Beat(96), tap->Beat(108), "eyes_rings"));

	er->AddEffect(std::make_shared< GroupSolid>(harness, tap->Beat(112), tap->Beat(120), "eyes_angry"));
	er->AddEffect(std::make_shared< GroupSolid>(harness, tap->Beat(120), tap->Beat(124), "eyes_v"));
	er->AddEffect(std::make_shared< GroupSolid>(harness, tap->Beat(124), tap->Beat(132), "eyes_big"));
	er->AddEffect(std::make_shared< GroupSolid>(harness, tap->Beat(132), tap->Beat(144), "eyes_big"));






	//	0	
	{
		er->AddEffect(std::make_shared<GroupSolid>(harness, tap->Beat(0), tap->Beats(1), "right_paw_1", BLUE));
		er->AddEffect(std::make_shared<GroupSolid>(harness, tap->Beat(1), tap->Beats(1), "left_paw_1", BLUE));
		er->AddEffect(std::make_shared<GroupSolid>(harness, tap->Beat(2), tap->Beats(1), "right_paw_2", BLUE));
		er->AddEffect(std::make_shared<GroupSolid>(harness, tap->Beat(2.5), tap->Beats(1), "left_paw_2", BLUE));
		er->AddEffect(std::make_shared<GroupSolid>(harness, tap->Beat(3), tap->Beats(1), "right_paw_3", BLUE));
		er->AddEffect(std::make_shared<GroupSolid>(harness, tap->Beat(3.5), tap->Beats(1), "left_paw_3", BLUE));
	}
	// 4
	{
		er->AddEffect(std::make_shared<GroupSolid>(harness, tap->Beat(4), tap->Beats(1), "right_paw_4", BLUE));
		er->AddEffect(std::make_shared<GroupSolid>(harness, tap->Beat(5), tap->Beats(1), "left_paw_4", BLUE));
		er->AddEffect(std::make_shared<GroupSolid>(harness, tap->Beat(6), tap->Beats(1), "right_paw_5", BLUE));
		er->AddEffect(std::make_shared<GroupSolid>(harness, tap->Beat(7), tap->Beats(1), "left_paw_5", BLUE));
	}
	//	8	like a small boat
	{
		er->AddEffect(std::make_shared<GroupSolid>(harness, tap->Beat(8), tap->Beat(12), "left_hand", BLUE));
		er->AddEffect(std::make_shared<GroupSolid>(harness, tap->Beat(8), tap->Beats(12), "right_hand", BLUE));

	}
	//	12 on the oc-
	{
		auto twinkle = std::make_shared<Effect>(harness, tap->Beat(11), tap->Beat(15));
		auto twinkleMask = std::make_shared<RandomMask>(harness, 0.0f);
		twinkleMask->AddDriver([twinkleMask, tap](timestamp t)
			{
				Ease<float>(&twinkleMask->m_prob, t, 0.0f, 0.1f, tap->Beat(11), tap->Beat(13));
				Ease<float>(&twinkleMask->m_prob, t, 0.1f, 0.0f, tap->Beat(13), tap->Beat(15));
			}
		);
		twinkle->SetMask(twinkleMask);
		twinkle->SetMask(std::make_shared<GroupMask>(harness, "left_hand"));
		twinkle->SetTexture(blueTexture);

		er->AddEffect(twinkle);
	}
	//	16	-an, sending big wav-
	{

		{
			auto handWash = std::make_shared<Effect>(harness, tap->Beat(15), tap->Beat(17));
			auto handwashMask = std::make_shared<BandMask>(harness, 0, x_axis, 100);
			handwashMask->AddDriver([handwashMask, harness, tap](timestamp t) {
				Ease<int>(&handwashMask->m_center, t, 0, harness->GetMarker("marker_right_hand").x, tap->Beat(15), tap->Beat(17));
				}
			);

			handWash->SetTexture(blueTexture);
			handWash->SetMask(std::make_shared<GroupMask>(harness, "right_arm"));
			handWash->SetMask(handwashMask);

			er->AddEffect(handWash);
		}

		{
			auto handWash = std::make_shared<Effect>(harness, tap->Beat(15), tap->Beat(17));
			auto handwashMask = std::make_shared<BandMask>(harness, 0, x_axis, 100);
			handwashMask->AddDriver([handwashMask, harness, tap](timestamp t) {
				Ease<int>(&handwashMask->m_center, t, 0, harness->GetMarker("marker_left_hand").x, tap->Beat(15), tap->Beat(17));
				}
			);

			handWash->SetTexture(blueTexture);
			handWash->SetMask(std::make_shared<GroupMask>(harness, "left_arm"));
			handWash->SetMask(handwashMask);

			er->AddEffect(handWash);
		}

	}
	// 20 -es into mot-
	{

		{
			auto handWash = std::make_shared<Effect>(harness, tap->Beat(19), tap->Beat(22));
			auto handwashMask = std::make_shared<BandMask>(harness, 0, x_axis, 100);
			handwashMask->AddDriver([handwashMask, harness, tap](timestamp t) {
				Ease<int>(&handwashMask->m_center, t, harness->GetMarker("marker_right_hand").x, harness->GetMarker("marker_left_hand").x, tap->Beat(19), tap->Beat(22), EaseType::BEIZIER);
				}
			);

			handWash->SetTexture(blueTexture);
			handWash->SetMask(std::make_shared<GroupMask>(harness, "arms"));
			handWash->SetMask(handwashMask);

			er->AddEffect(handWash);
		}

	}
	// 24 -ion, like how a single
	{
		{ // white hand orb
			auto orbEffect = std::make_shared<Effect>(harness, tap->Beat(24), tap->Beat(30));
			auto orbMask = std::make_shared<OrbMask>(harness, harness->GetMarker("marker_right_hand"), 300);

			orbMask->AddDriver([orbMask, tap](timestamp t) {
				Ease<float>(&orbMask->m_intensity, t, 0.0f, 1.0f, tap->Beat(22), tap->Beat(24));
				Ease<float>(&orbMask->m_intensity, t, 1.0f, 0.0f, tap->Beat(24), tap->Beat(30));
				});

			orbEffect->SetMask(orbMask);
			orbEffect->SetTexture(std::make_shared<SolidTexture>(harness, WHITE));

			er->AddEffect(orbEffect);
		}
	}
	// 28 word can make a heart o-
	{
		auto heartEffect = std::make_shared<Effect>(harness, tap->Beat(28), tap->Beat(32));
		auto heartMask = std::make_shared<GroupMask>(harness, "heart");
		heartMask->AddDriver([heartMask, tap](timestamp t) {
			Ease<float>(&heartMask->m_intensity, t, 0.0f, 1.0f, tap->Beat(28), tap->Beat(30));
			Ease<float>(&heartMask->m_intensity, t, 1.0f, 0.0f, tap->Beat(30), tap->Beat(34));
			}
		);
		heartEffect->SetTexture(std::make_shared<SolidTexture>(harness, RED));
		heartEffect->SetMask(heartMask);

		er->AddEffect(heartEffect);
	}
	// 30 pen, I might only have
	{
	}

	//	34	one match that i can make an explosion
	{
		// fire hand orb
		auto orbEffect = std::make_shared<Effect>(harness, tap->Beat(32), tap->Beat(36));
		auto orbMask = std::make_shared<OrbMask>(harness, harness->GetMarker("marker_right_hand"), 300);

		orbMask->AddDriver([orbMask, harness, tap](timestamp t) {
			LocEase(&orbMask->m_center, t, harness->GetMarker("marker_right_hand"), harness->GetMarker("marker_left_hand"), tap->Beat(35.5), tap->Beat(36));
			});

		orbEffect->SetMask(orbMask);
		orbEffect->SetTexture(fireTexture);

		er->AddEffect(orbEffect);

	}

	//	36	-plosion and all the

	{
		auto fireballEffect = std::make_shared<Effect>(harness, tap->Beat(36), tap->Beat(40));
		auto fadeMask = std::make_shared< GlowMask>(harness, harness->GetMarker("marker_left_hand"), 1000);

		fadeMask->AddDriver([fadeMask, tap](timestamp t) {
			Ease<int>(&fadeMask->m_maxDistance, t, 1000, 0, tap->Beat(37), tap->Beat(39));
			}
		);

		fireballEffect->SetMask(fadeMask);

		fireballEffect->SetTexture(fireTexture);

		er->AddEffect(fireballEffect);
	}

	{ // fire hand orb
		auto orbEffect = std::make_shared<Effect>(harness, tap->Beat(40.5), tap->Beat(41));
		orbEffect->SetMask(std::make_shared<OrbMask>(harness, harness->GetMarker("marker_right_hand"), 300));
		orbEffect->SetTexture(fireTexture);
		er->AddEffect(orbEffect);
	}

	//	40	things i didn't say

	{
		{ // fire hand orb left
			auto orbEffect = std::make_shared<Effect>(harness, tap->Beat(40), tap->Beat(40.5));
			orbEffect->SetTexture(fireTexture);

			auto orbMask = std::make_shared<OrbMask>(harness, harness->GetMarker("marker_left_hip"), 300);

			orbMask->AddDriver([orbMask, harness, tap](timestamp t) {
				LocEase(&orbMask->m_center, t, harness->GetMarker("marker_left_hip"), harness->GetMarker("marker_left_foot"), tap->Beat(40), tap->Beat(40.5));
				});

			orbEffect->SetMask(orbMask);

			er->AddEffect(orbEffect);
		}

		{ // fire hand orb right
			auto orbEffect = std::make_shared<Effect>(harness, tap->Beat(41), tap->Beat(41.5));
			orbEffect->SetTexture(fireTexture);

			auto orbMask = std::make_shared<OrbMask>(harness, harness->GetMarker("marker_right_hip"), 300);

			orbMask->AddDriver([orbMask, harness, tap](timestamp t) {
				LocEase(&orbMask->m_center, t, harness->GetMarker("marker_right_hip"), harness->GetMarker("marker_right_foot"), tap->Beat(41), tap->Beat(41.5));
				});

			orbEffect->SetMask(orbMask);

			er->AddEffect(orbEffect);
		}
	}

	//	42 ish	were wrecking balls
	//		inside my
	//		brain

	//		I will
	//	48	scream

	{
		auto orbEffect = std::make_shared<Effect>(harness, tap->Beat(48), tap->Beat(56));
		auto orbMask = std::make_shared<OrbMask>(harness, harness->GetMarker("marker_right_hand"), 300);

		orbEffect->SetMask(orbMask);

		auto blueFireTexture = std::make_shared<SolidTexture>(harness, BLUE);
		fireTexture->AddDriver([blueFireTexture, tap](timestamp t) {
			Cycle<float>(&blueFireTexture->m_hsv.h, t, 0.66f, 0.8f, tap->Beat(0), tap->Beats(0), CycleType::RANDOM);
			});


		orbEffect->SetTexture(blueFireTexture);

		er->AddEffect(orbEffect);
	}

	//		em out
	//		loud tonight
	//		can you
	//	52	hear
	//		my voice
	//		this time
	//		this is my
	{
		{ // white left hand orb
			auto orbEffect = std::make_shared<Effect>(harness, tap->Beat(42), tap->Beat(54));
			orbEffect->SetTexture(std::make_shared<SolidTexture>(harness, WHITE));
			auto orbMask = std::make_shared<OrbMask>(harness, harness->GetMarker("marker_right_hand"));
			orbMask->AddDriver([orbMask, tap](timestamp t) {
				Ease<int>(&orbMask->m_diameter, t, 300, 1500, tap->Beat(42), tap->Beat(52));
				Ease<int>(&orbMask->m_diameter, t, 1500, 0, tap->Beat(52), tap->Beat(54));
				});
			orbEffect->SetMask(orbMask);
			er->AddEffect(orbEffect);
		}

		{ // white right hand orb
			auto orbEffect = std::make_shared<Effect>(harness, tap->Beat(43), tap->Beat(54));
			orbEffect->SetTexture(std::make_shared<SolidTexture>(harness, WHITE));
			auto orbMask = std::make_shared<OrbMask>(harness, harness->GetMarker("marker_left_hand"));
			orbMask->AddDriver([orbMask, tap](timestamp t) {
				Ease<int>(&orbMask->m_diameter, t, 300, 1500, tap->Beat(43), tap->Beat(52));
				Ease<int>(&orbMask->m_diameter, t, 1500, 0, tap->Beat(52), tap->Beat(54));
				});
			orbEffect->SetMask(orbMask);
			er->AddEffect(orbEffect);
		}

		// distortion time!
		{
			auto distortEffect = std::make_shared<Effect>(harness, tap->Beat(42), tap->Beat(52));
			auto blackTexture = std::make_shared<SolidTexture>(harness, BLACK);
			distortEffect->SetTexture(blackTexture);

			auto randomMask = std::make_shared<RandomMask>(harness);
			randomMask->AddDriver([randomMask, tap](timestamp t) {
				Ease<float>(&randomMask->m_prob, t, 0.0f, 0.5f, tap->Beat(42), tap->Beat(52));
				}
			);
			distortEffect->SetMask(randomMask);

			er->AddEffect(distortEffect);
		}
	}


	//56 punch fight
	{
		er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(56), tap->Beats(1), harness->GetMarker("marker_right_hand"), BLUE, 2000, 100, false));
	}

	//56.5 punch song
	{
		er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(56.5), tap->Beats(1), harness->GetMarker("marker_left_hand"), BLUE, 2000, 100, false));

	}

	//57 wipe back
	{
		auto leftArmFill = std::make_shared<Effect>(harness, tap->Beat(57), tap->Beat(57.5));

		leftArmFill->SetTexture(std::make_shared<SolidTexture>(harness, BLUE));

		leftArmFill->SetMask(std::make_shared<GroupMask>(harness, "left_arm"));

		er->AddEffect(leftArmFill);

	}

	//58 take
	//59 back my

	//60 life
	{
		auto rightArmFill = std::make_shared<Effect>(harness, tap->Beat(60), tap->Beat(61));
		rightArmFill->SetTexture(std::make_shared<SolidTexture>(harness, BLUE));
		rightArmFill->SetMask(std::make_shared<GroupMask>(harness, "right_arm"));
		er->AddEffect(rightArmFill);

		er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(60), tap->Beats(4), harness->GetMarker("marker_right_hand"), WHITE, 2000, 100, true));
	}
	//60.5 left
	{
		auto leftArmFill = std::make_shared<Effect>(harness, tap->Beat(60.5), tap->Beat(61));
		leftArmFill->SetTexture(std::make_shared<SolidTexture>(harness, BLUE));
		leftArmFill->SetMask(std::make_shared<GroupMask>(harness, "left_arm"));
		er->AddEffect(leftArmFill);
		er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(60.5), tap->Beats(4), harness->GetMarker("marker_left_hand"), WHITE, 2000, 100, true));
	}
	//61 song
	{}
	// 62 prove

	// 63 im all

	//64 right strike

	{
		auto handWash = std::make_shared<Effect>(harness, tap->Beat(64), tap->Beat(65));
		auto handwashMask = std::make_shared<BandMask>(harness, 0, x_axis, 100);
		handwashMask->AddDriver([handwashMask, harness, tap](timestamp t) {
			Ease<int>(&handwashMask->m_center, t, 0, harness->GetMarker("marker_right_hand").x, tap->Beat(64), tap->Beat(65));
			}
		);
		handWash->SetTexture(blueTexture);
		handWash->SetMask(handwashMask);

		er->AddEffect(handWash);
	}

	//64.5 left strike 

	{
		auto handWash = std::make_shared<Effect>(harness, tap->Beat(64.5), tap->Beat(65.5));
		auto handwashMask = std::make_shared<BandMask>(harness, 0, x_axis, 100);
		handwashMask->AddDriver([handwashMask, harness, tap](timestamp t) {
			Ease<int>(&handwashMask->m_center, t, 0, harness->GetMarker("marker_left_hand").x, tap->Beat(64.5), tap->Beat(65.5));
			}
		);
		handWash->SetTexture(blueTexture);
		handWash->SetMask(handwashMask);

		er->AddEffect(handWash);
	}

	//65 wipe sooooo
	{
		auto radialBlueSpin = std::make_shared<Effect>(harness, tap->Beat(65), tap->Beat(70));
		auto radialMask = std::make_shared<RadialMask>(harness);
		radialBlueSpin->SetTexture(std::make_shared<SolidTexture>(harness, BLUE));

		radialMask->AddDriver([radialMask, tap](timestamp t) {
			Ease<float>(&radialMask->m_offset, t, 0.0f, 10.0f, tap->Beat(65), tap->Beat(70));
			});

		radialBlueSpin->SetMask(radialMask);
		er->AddEffect(radialBlueSpin);
	}
	//68 end wipe ng

	//70 my 
	// 71 powers 

	// 72 turned #
	{
		er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(72), tap->Beats(1), harness->GetMarker("marker_right_hand"), BLUE, 1500, 200, true));
		er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(72), tap->Beats(1), harness->GetMarker("marker_left_hand"), BLUE, 1500, 200, true));
	}


	// 73 on st

	{
		auto chestGlowEffect = std::make_shared<Effect>(harness, tap->Beat(73), tap->Beat(76));
		auto fadeMask = std::make_shared< GlowMask>(harness, harness->GetMarker("marker_chest"), 1000);

		fadeMask->AddDriver([fadeMask, tap](timestamp t) {
			Ease<int>(&fadeMask->m_maxDistance, t, 1000, 0, tap->Beat(73), tap->Beat(76));
			}
		);

		chestGlowEffect->SetMask(fadeMask);

		chestGlowEffect->SetTexture(std::make_shared<SolidTexture>(harness, BLUE));

		er->AddEffect(chestGlowEffect);
	}

	// 74 arting right

	// 75 now ill

	// 76 be 
	{
		auto rightArmFill = std::make_shared<Effect>(harness, tap->Beat(76), tap->Beat(78));
		rightArmFill->SetTexture(std::make_shared<SolidTexture>(harness, WHITE));
		rightArmFill->SetMask(std::make_shared<GroupMask>(harness, "right_arm"));
		er->AddEffect(rightArmFill);
	}

	// 77 strong

	{
		auto leftArmFill = std::make_shared<Effect>(harness, tap->Beat(77), tap->Beat(78));
		leftArmFill->SetTexture(std::make_shared<SolidTexture>(harness, WHITE));
		leftArmFill->SetMask(std::make_shared<GroupMask>(harness, "left_arm"));
		er->AddEffect(leftArmFill);
	}

	// 78 ill

	// 79 play my

	// 80 fight
	{
		er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(80), tap->Beats(2), harness->GetMarker("marker_right_hand"), BLUE, 2000, 200, false));
		er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(80.5), tap->Beats(2), harness->GetMarker("marker_left_hand"), BLUE, 2000, 200, false));
	}

	// 81 song
	{
		auto leftArmFill = std::make_shared<Effect>(harness, tap->Beat(81), tap->Beat(81.5));
		leftArmFill->SetTexture(std::make_shared<SolidTexture>(harness, BLUE));
		leftArmFill->SetMask(std::make_shared<GroupMask>(harness, "left_arm"));
		er->AddEffect(leftArmFill);
	}

	// 83 inna

	// 84 don really

	{ // white hand orb
		auto orbEffect = std::make_shared<Effect>(harness, tap->Beat(84), tap->Beat(84.5));
		orbEffect->SetMask(std::make_shared<OrbMask>(harness, harness->GetMarker("marker_right_hand"), 300));
		orbEffect->SetTexture(std::make_shared<SolidTexture>(harness, WHITE));

		er->AddEffect(orbEffect);
	}

	// 85 care if

	// 86 no body

	{ // white hand orb
		auto orbEffect = std::make_shared<Effect>(harness, tap->Beat(86), tap->Beat(86.5));
		orbEffect->SetMask(std::make_shared<OrbMask>(harness, harness->GetMarker("marker_right_hand"), 300));
		orbEffect->SetTexture(std::make_shared<SolidTexture>(harness, WHITE));

		er->AddEffect(orbEffect);
	}

	// 87 else be

	// 88 liee // reach back

	{
		auto handWash = std::make_shared<Effect>(harness, tap->Beat(88), tap->Beat(92));
		auto handwashMask = std::make_shared<BandMask>(harness, 0, x_axis, 100);

		handwashMask->AddDriver([handwashMask, harness, tap](timestamp t) {
			Ease<int>(&handwashMask->m_center, t, 0, harness->GetMarker("marker_right_hand").x, tap->Beat(88), tap->Beat(90), EaseType::BEIZIER);
			Ease<int>(&handwashMask->m_center, t, harness->GetMarker("marker_right_hand").x, 0, tap->Beat(90), tap->Beat(92), EaseType::BEIZIER);

			}
		);

		handWash->SetMask(handwashMask);
		handWash->SetMask(std::make_shared<GroupMask>(harness, "arms"));

		handWash->SetTexture(std::make_shared<LinearRainbowTexture>(harness, harness->GetMarker("marker_chest"), x_axis));

		er->AddEffect(handWash);
	}

	// 91 -vs cause

	// 92 i've still

	// 93  got

	// 94 allotta fight

	// 95 is my 

	// 96 fight (hit)

	{
		//persistent rainbow thing

		auto rainbowSpinEffect = std::make_shared< RainbowSpin>(harness, harness->GetMarker("marker_chest"), tap->Beat(96.5), tap->Beat(108), y_axis);
		auto orbMask = std::make_shared<OrbMask>(harness, harness->GetMarker("marker_chest"), 100);
		orbMask->AddDriver([orbMask, tap](timestamp t) {
			Ease<int>(&orbMask->m_diameter, t, 100, 200, tap->Beat(96.5), tap->Beat(97));
			Ease<int>(&orbMask->m_diameter, t, 200, 300, tap->Beat(100.5), tap->Beat(101));
			Ease<int>(&orbMask->m_diameter, t, 300, 400, tap->Beat(104.5), tap->Beat(105));
			});

		rainbowSpinEffect->SetMask(orbMask);

		er->AddEffect(rainbowSpinEffect);
	}

	// 96.5 (hit)

	er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(96), tap->Beats(1), harness->GetMarker("marker_right_hand"), RED, 2000, 200, false));
	er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(96.5), tap->Beats(1), harness->GetMarker("marker_left_hand"), RED, 2000, 200, false));


	// 97 song

	// 98 take

	// 99 back my

	// 100 life (hit)

	// 100.5 (hit)

	er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(100), tap->Beats(1), harness->GetMarker("marker_right_hand"), GREEN, 2000, 200, false));
	er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(100.5), tap->Beats(1), harness->GetMarker("marker_left_hand"), GREEN, 2000, 200, false));


	// 101 song

	// 102 prove

	// 103 im all

	// 104 right (hit)

	// 104.5 (hit)

	er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(104), tap->Beats(1), harness->GetMarker("marker_right_hand"), BLUE, 2000, 200, false));
	er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(104.5), tap->Beats(1), harness->GetMarker("marker_left_hand"), BLUE, 2000, 200, false));

	// 105 so

	// 108 ng (hit)

	// 108.5 (hit)

	// 109 (hit)

	// 109.5 (hit)

	// 110 my (hit)

	er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(108), tap->Beats(1), harness->GetMarker("marker_chest"), RED, 2000, 200, false));
	er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(108.5), tap->Beats(1), harness->GetMarker("marker_chest"), GREEN, 2000, 200, false));
	er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(109), tap->Beats(1), harness->GetMarker("marker_chest"), BLUE, 2000, 200, false));
	er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(109.5), tap->Beats(1), harness->GetMarker("marker_chest"), GREY, 2000, 200, false));
	er->AddEffect(std::make_shared<Ripple>(harness, tap->Beat(110), tap->Beats(1), harness->GetMarker("marker_chest"), WHITE, 2000, 200, false));


	// 111 powers

	// 112 turned (big drop)

	{
		auto rainbowStaticEffect = std::make_shared<Effect>(harness, tap->Beat(112), tap->Beat(114));
		rainbowStaticEffect->SetTexture(std::make_shared<RadialRainbowTexture>(harness, harness->GetMarker("marker_chest"), y_axis));
		er->AddEffect(rainbowStaticEffect);
	}

	er->AddEffect(std::make_shared<DimmerEffect>(harness, tap->Beat(112), tap->Beats(1), 0.75f));

	// 113 on

	for (int beat = 113; beat < 134; ++beat)
		er->AddEffect(std::make_shared<RainbowRipple>(harness, tap->Beat(beat), tap->Beats(3), harness->GetRandomLoc(), 1000, 200));


	// 114 starting right

	// 115 now ill

	// 116 be str
	{
		auto rightArmFill = std::make_shared<Effect>(harness, tap->Beat(116), tap->Beat(118));
		rightArmFill->SetTexture(std::make_shared<RadialRainbowTexture>(harness, harness->GetMarker("marker_right_bicep"), y_axis));
		rightArmFill->SetMask(std::make_shared<GroupMask>(harness, "right_arm"));
		er->AddEffect(rightArmFill);
	}

// 77 strong

	{
		auto rightArmFill = std::make_shared<Effect>(harness, tap->Beat(117), tap->Beat(118));
		rightArmFill->SetTexture(std::make_shared<RadialRainbowTexture>(harness, harness->GetMarker("marker_left_bicep"), y_axis));
		rightArmFill->SetMask(std::make_shared<GroupMask>(harness, "left_arm"));
		er->AddEffect(rightArmFill);
	}

	er->AddEffect(std::make_shared<RainbowRipple>(harness, tap->Beat(116), tap->Beats(2), harness->GetMarker("marker_right_bicep"), 2000, 200, false));
	// 117 ong
	er->AddEffect(std::make_shared<RainbowRipple>(harness, tap->Beat(117), tap->Beats(2), harness->GetMarker("marker_left_bicep"), 2000, 200, false));

	// 118 ill

	// 119 play my

	// 120 fight

	er->AddEffect(std::make_shared<RainbowRipple>(harness, tap->Beat(120), tap->Beats(1), harness->GetMarker("marker_left_hand"), 2000, 200, false));
	er->AddEffect(std::make_shared<RainbowRipple>(harness, tap->Beat(120.5), tap->Beats(1), harness->GetMarker("marker_right_hand"), 2000, 200, false));

	// 121 song

	er->AddEffect(std::make_shared<RainbowRipple>(harness, tap->Beat(121), tap->Beats(2), harness->GetMarker("marker_chest"), 2000, 200, false));

	// 123 anna

	// 124 don't really

	{
		auto a = std::make_shared<RainbowSpin>(harness, harness->GetMarker("center"), tap->Beat(128), tap->Beat(134), z_axis, 2.0f);
		auto m = std::make_shared< GroupMask>(harness, "main");
		m->AddDriver([m, tap](timestamp t) {
			Ease<float>(&m->m_intensity, t, 1.0f, 0.0f, tap->Beat(132), tap->Beat(140));
			});
		a->SetMask(m);

		er->AddEffect(a);
	}


	er->AddEffect(std::make_shared<RainbowRipple>(harness, tap->Beat(124), tap->Beats(4), harness->GetMarker("marker_right_hand"), 2000, 200, false));

	// 125 care if

	// 126 nobody

	er->AddEffect(std::make_shared<RainbowRipple>(harness, tap->Beat(126), tap->Beats(4), harness->GetMarker("marker_left_hand"), 2000, 200, false));


	// 127 else be-

	// 128 -lieve-



	// 131 -s cause

	// 132 i've still

	er->AddEffect(std::make_shared<RainbowRipple>(harness, tap->Beat(132), tap->Beats(2), harness->GetMarker("marker_left_hand"), 2000, 200, false));

	// 133 got a

	// 134 lotta fight

	er->AddEffect(std::make_shared<RainbowRipple>(harness, tap->Beat(134), tap->Beats(5), harness->GetMarker("marker_right_hand"), 2000, 200, false));
	er->AddEffect(std::make_shared<RainbowRipple>(harness, tap->Beat(134), tap->Beats(5), harness->GetMarker("marker_chest"), 2000, 200, false));


	// 135 left in

	// 136 me

	// 139 no

	// 140 i've still got a lotta fight left in me


	er->AddEffect(std::make_shared<BoostEffect>(harness, tap->Beat(0), tap->Beat(144), tap));
}
