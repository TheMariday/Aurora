#include "tef/aurora/masterController.h"
#include "Effect Driver Project/FightSong.h"
#include "Effect Driver Project/Eyes.h"
#include "Effect Driver Project/Easer.h"

#define NOSE Loc(0, -330, 567)


void TEF::Aurora::MasterController::SetupVoiceCommands()
{
	m_userControl.RegisterVoid("cancel that", INSTANT, [this]() {
		UnloadCommand();
		return "Command canceled";
		});

	m_userControl.RegisterVoid("whats loaded", INSTANT, [this]()
		{
			if (!m_loadedCommand)
				return std::string("No command loaded");
			std::stringstream ss;
			ss << "Loaded command " << m_loadedCommand->GetCommandAndArgs();
			return ss.str();
		});

	/*
	m_userControl.RegisterVoid("clear fault", CONFIRM, [this]()
		{
			ClearFault();
			return "fault cleared";
		});

	m_userControl.RegisterVoid("fault", INSTANT, [this]()
		{
			CriticalFault();
			return "";
		});

	m_userControl.RegisterVoid("get active fault", INSTANT, [this]()
		{
			std::stringstream ss;
			if (m_fault)
				ss << m_faultError.str();
			else
				ss << "There are no faults. ";
			return ss.str();

		});

	*/
	m_userControl.RegisterVoid("are you ready", INSTANT, [this]()
		{
			return "I am ready, lets do this!";
		});

	m_userControl.RegisterVoid("hello", INSTANT, [this]()
		{
			return "Hello! I am Highbeams Voice Automated Control system, or h vac for short. To see what commands you can use, please refer to my specification sheet next to me. Simply hold the blue button, say the command, then release. Use the green button to confirm your command when prompted.";
		});

	m_userControl.RegisterVoid("statistics", CONFIRM, [this]()
		{
			return "Highbeam, Fullsuit class, Version 4. LED count, three thousand one hundred and thirty six. Maximum current delivery, one hundred and sixty amps. Sound system, stereo 10 watt drivers. Battery life : 1 hour. Current maximum bid. Unknown.";
		});

	/*
	m_userControl.RegisterVoid("get battery levels", INSTANT, [this]()
		{
			std::vector<Cell> cells;
			m_batteryMonitor.GetCells(cells);
			std::stringstream ss;
			for (auto cell : cells)
				ss << "Cell " << cell.cellIndex << " at " << std::to_string(cell.currentVoltage).substr(0, 4) << " volts. ";

			return ss.str();
		});

	m_userControl.RegisterVoid("get headset volume", INSTANT, [this]()
		{
			int volume = static_cast<int>(m_headset.GetVolume() * 10);

			std::stringstream ss;
			ss << "headset volume at " << volume;
			return ss.str();
		});

	m_userControl.RegisterLimitedInt("set headset volume to", CONFIRM, [this](int volume)
		{
			m_headset.SetVolume(static_cast<float>(volume) / 10.0f);
			std::stringstream ss;
			ss << "headset volume set to " << volume;
			return ss.str();
		});


	m_userControl.RegisterVoid("get tail volume", INSTANT, [this]()
		{
			int volume = static_cast<int>(m_tailbass.GetVolume() * 10);

			std::stringstream ss;
			ss << "tail volume at " << volume;
			return ss.str();
		});

	m_userControl.RegisterLimitedInt("set tail volume to", CONFIRM, [this](int volume)
		{
			m_tailbass.SetVolume(static_cast<float>(volume) / 10.0f);
			std::stringstream ss;
			ss << "tail volume set to " << volume;
			return ss.str();
		});

	m_userControl.RegisterVoid("stop tail audio", CONFIRM, [this]()
		{
			m_tailbass.Stop();
			return "";
		});
    */
	m_userControl.RegisterVoid("stop", CONFIRM, [this]()
		{
			m_tailbass.Stop();
			m_effectRunner.StopAll();
			return "";
		});

	m_userControl.RegisterVoid("say are we friends", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/are_we_friends_now.wav");
			return "";
		});

	m_userControl.RegisterVoid("say cant", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/cant.wav");
			return "";
		});

	m_userControl.RegisterVoid("say cant hear", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/cant_hear.wav");
			return "";
		});

	m_userControl.RegisterVoid("say cant hug", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/cant_hug.wav");
			return "";
		});

	m_userControl.RegisterVoid("say diodes", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/diodes.wav");
			return "";
		});

	m_userControl.RegisterVoid("say gaze", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/gaze_into_the_iris.wav");
			m_effectRunner.AddEffect(std::make_shared< GroupSolid>(&m_effectRunner.m_harness, Now(), Now() + std::chrono::seconds(2), "main", YELLOW));

			return "";
		});

	m_userControl.RegisterVoid("say ghosts", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/ghosts_in_the_machine.wav");
			return "";
		});

	m_userControl.RegisterVoid("say goodbye", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/goodbye.wav");
			return "";
		});

	m_userControl.RegisterVoid("say have a safe day", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/have_a_safe_day.wav");
			return "";
		});

	m_userControl.RegisterVoid("say hello friend", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/hello_friend.wav");
			SetEyes(&m_effectRunner, Now(), std::chrono::seconds(5), "eyes_heart", WHITE);

			return "";
		});

	m_userControl.RegisterVoid("say violence", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/i_am_programmed_to_avoid_violence.wav");
			SetEyes(&m_effectRunner, Now() + std::chrono::seconds(3), std::chrono::seconds(5), "eyes_angry", RED);
			return "";
		});

	m_userControl.RegisterVoid("play joe joe", CONFIRM, [this]()
		{
			SetEyes(&m_effectRunner, Now() + std::chrono::milliseconds(2500), std::chrono::seconds(5), "eyes_lids", YELLOW);
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/jojo.wav");
			return "";
		});

	m_userControl.RegisterVoid("say mystery", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/life_can_be_mysterious_at_times.wav");
			return "";
		});

	m_userControl.RegisterVoid("say limited voice", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/limited_vocab.wav");
			return "";
		});

	m_userControl.RegisterVoid("say creators", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/my_creators.wav");
			return "";
		});

	m_userControl.RegisterVoid("say my name", CONFIRM, [this]()
		{
			m_effectRunner.AddEffect(std::make_shared< GroupSolid>(&m_effectRunner.m_harness, Now(), Now() + std::chrono::seconds(5), "heart", RED));

			m_tailbass.PlayAudio("/home/pi/media/voice_lines/my_name_is_highbeam.wav");
			return "";
		});

	m_userControl.RegisterVoid("say my pleasure", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/my_pleasure.wav");
			return "";
		});

	m_userControl.RegisterVoid("say no", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/no.wav");
			return "";
		});

	m_userControl.RegisterVoid("say not fast", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/not_fast.wav");
			return "";
		});

	m_userControl.RegisterVoid("say stand clear", CONFIRM, [this]()
		{
			timestamp now = Now();
			m_effectRunner.AddEffect(std::make_shared< GroupSolid>(&m_effectRunner.m_harness, now, now + std::chrono::seconds(3), "left_hand", RED));
			m_effectRunner.AddEffect(std::make_shared< GroupSolid>(&m_effectRunner.m_harness, now, now + std::chrono::seconds(3), "right_hand", RED));

			m_tailbass.PlayAudio("/home/pi/media/voice_lines/please_stand_clear.wav");
			return "";
		});

	m_userControl.RegisterVoid("say four hundred percent", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/power_at_four_hundred_percent.wav");
			return "";
		});

	m_userControl.RegisterVoid("say powered up", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/powered_up_and_ready_to_go.wav");
			return "";
		});

	m_userControl.RegisterVoid("say overheat", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/system_overheat.wav");
			return "";
		});

	m_userControl.RegisterVoid("say understood", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/understood.wav");
			return "";
		});

	m_userControl.RegisterVoid("say yes", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/yes.wav");
			return "";
		});

	m_userControl.RegisterVoid("say perimeter breach", CONFIRM, [this]()
		{
			timestamp now = Now();
			m_effectRunner.AddEffect(std::make_shared< GroupSolid>(&m_effectRunner.m_harness, now, now + std::chrono::seconds(3), "left_hand", RED));
			m_effectRunner.AddEffect(std::make_shared< GroupSolid>(&m_effectRunner.m_harness, now, now + std::chrono::seconds(3), "right_hand", RED));

			SetEyes(&m_effectRunner, now, std::chrono::seconds(6), "eyes_cross", RED);

			m_tailbass.PlayAudio("/home/pi/media/voice_lines/perimeter_breach.wav");
			return "";
		});

	
	m_userControl.RegisterVoid("system restart", CONFIRM, [this]()
		{
			m_headset.PlayAudio("/home/pi/media/cyclops/AI_engine_down.wav", true);

			m_quit = true;
			return "";
		});
	

	m_userControl.RegisterVoid("play blood hound", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/sfx/bloodhound.wav");

			timestamp now = Now();
			timestamp drop = now + std::chrono::milliseconds(1500);
			timestamp end = now + std::chrono::seconds(7);

			SetEyes(&m_effectRunner, drop, std::chrono::seconds(6), "eyes_angry", RED);

			m_effectRunner.AddEffect(std::make_shared< GroupSolid>(&m_effectRunner.m_harness, now, end, "left_eye", RED));
			m_effectRunner.AddEffect(std::make_shared< GroupSolid>(&m_effectRunner.m_harness, now, end, "right_eye", RED));

			m_effectRunner.AddEffect(std::make_shared< GroupSolid>(&m_effectRunner.m_harness, drop, std::chrono::seconds(1), "head", RED));


			return "";
		});

	m_userControl.RegisterVoid("play blue screen", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/sfx/bluescreen.wav");
			m_effectRunner.AddEffect(std::make_shared< GroupSolid>(&m_effectRunner.m_harness, Now(), std::chrono::seconds(1), "head", BLUE));
			return "";
		});

	m_userControl.RegisterVoid("play electric", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/sfx/electric.wav");
			auto start = Now();
			auto end = start + std::chrono::seconds(1);
			SetEyes(&m_effectRunner, start, end, "eyes_big", BLUE);

			auto twinkle = std::make_shared<Effect>(&m_effectRunner.m_harness, start, end);
			auto twinkleMask = std::make_shared<RandomMask>(&m_effectRunner.m_harness);

			twinkle->SetMask(twinkleMask);
			twinkle->SetTexture(std::make_shared<SolidTexture>(&m_effectRunner.m_harness, WHITE));

			m_effectRunner.AddEffect(twinkle);

			return "";
		});

	m_userControl.RegisterVoid("play growl", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/sfx/growl.wav");
			SetEyes(&m_effectRunner, Now(), std::chrono::seconds(3), "eyes_angry", RED);
			return "";
		});

	m_userControl.RegisterVoid("play siren", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/sfx/siren.wav");

			timestamp now = Now();

			for (int i = 0; i < 3; ++i)
			{
				timestamp t = now + std::chrono::seconds(i);

				m_effectRunner.AddEffect(std::make_shared< GroupSolid>(&m_effectRunner.m_harness, t, std::chrono::milliseconds(50), "eye_left", RED));
				m_effectRunner.AddEffect(std::make_shared< GroupSolid>(&m_effectRunner.m_harness, t + std::chrono::milliseconds(150), std::chrono::milliseconds(50), "eye_left", RED));
				t += std::chrono::milliseconds(500);
				m_effectRunner.AddEffect(std::make_shared< GroupSolid>(&m_effectRunner.m_harness, t, std::chrono::milliseconds(50), "eye_right", RED));
				m_effectRunner.AddEffect(std::make_shared< GroupSolid>(&m_effectRunner.m_harness, t + std::chrono::milliseconds(150), std::chrono::milliseconds(50), "eye_right", BLUE));
			}


			return "";
		});

	m_userControl.RegisterVoid("play fight song", CONFIRM, [this]()
		{
			timestamp now = Now();
			m_tailbass.PlayAudio("/home/pi/media/sfx/fight_song.wav");
			FightSong(&m_effectRunner, &m_effectRunner.m_harness, now);

			return "good luck sir";
		});

	m_userControl.RegisterVoid("play wipe", CONFIRM, [this]()
		{
			timestamp now = Now();

			m_effectRunner.AddEffect(std::make_shared<CalibrationEffect>(&m_effectRunner.m_harness));

			return "wipe started";
		});

	m_userControl.RegisterVoid("play splash", INSTANT_TRIGGER, [this]()
		{
			timestamp now = Now();
			auto loc = m_effectRunner.m_harness.GetRandomLoc();
			m_effectRunner.AddEffect(std::make_shared<RainbowRipple>(&m_effectRunner.m_harness, now, std::chrono::seconds(1), loc, 1000, 200));

			return "";
		});


	m_userControl.RegisterVoid("play iron man", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/sfx/ironman.wav");
			auto start = Now();
			auto drop1 = start + std::chrono::milliseconds(268);
			auto drop2 = start + std::chrono::milliseconds(900);
			auto end = start + std::chrono::milliseconds(1816);

			SetEyes(&m_effectRunner, start, end, "eyes_cross", BLUE);

			Harness* harness = &m_effectRunner.m_harness;

			auto fireballEffect = std::make_shared<Effect>(harness, start, end);
			auto fadeMask = std::make_shared< GlowMask>(harness, NOSE, 200);

			fadeMask->AddDriver([fadeMask, drop1, drop2](timestamp t) {
				Ease<int>(&fadeMask->m_maxDistance, t, 100, 1000, drop1, drop2);
				}
			);

			fireballEffect->SetMask(fadeMask);

			fireballEffect->SetTexture(std::make_shared<SolidTexture>(harness, WHITE));

			m_effectRunner.AddEffect(fireballEffect);

			return "";

		});

	m_userControl.RegisterString("play magic", { "red", "green","blue", "rainbow" }, CONFIRM, [this](std::string col)
		{
			m_tailbass.PlayAudio("/home/pi/media/sfx/magic.wav");

			timestamp now = Now();

			Harness* harness = &m_effectRunner.m_harness;

			auto start = Now();
			auto drop = start + std::chrono::milliseconds(1500);

			auto end = start + std::chrono::milliseconds(5000);
#
			auto handshakeEffect = std::make_shared<Effect>(harness, start, end);

			auto m = std::make_shared< GlowMask>(harness, NOSE, 1000);

			m->AddDriver([m, start, drop](timestamp t) {
				Ease<int>(&m->m_maxDistance, t, 0, 2000, start, drop);
				}
			);

			handshakeEffect->SetMask(m);


			if (col == "red")
			{
				handshakeEffect->SetTexture(std::make_shared<SolidTexture>(harness, RED));
			}
			if (col == "green")
			{
				handshakeEffect->SetTexture(std::make_shared<SolidTexture>(harness, GREEN));
			}
			if (col == "blue")
			{
				handshakeEffect->SetTexture(std::make_shared<SolidTexture>(harness, BLUE));
			}
			if (col == "rainbow")
			{
				handshakeEffect->SetTexture(std::make_shared<RadialRainbowTexture>(harness, NOSE, y_axis));
			}

			m_effectRunner.AddEffect(handshakeEffect);

			return "";
		});



	m_userControl.RegisterVoid("play rainbow", INSTANT_TRIGGER, [this]()
		{
			timestamp now = Now();
			timestamp end = now + std::chrono::seconds(5);

			Harness* harness = &m_effectRunner.m_harness;

			auto a = std::make_shared<RainbowSpin>(harness, NOSE, now, end, y_axis, 2.0f);

			auto fadeMask = std::make_shared< GlowMask>(harness, NOSE, 1000);

			fadeMask->AddDriver([fadeMask, now, end](timestamp t) {
				Ease<int>(&fadeMask->m_maxDistance, t, 0, 2000, now, end);
				}
			);
			a->SetMask(fadeMask);

			m_effectRunner.AddEffect(a);

			return "";
		});

	m_userControl.RegisterString("set eyes", { "angry", "heart","cross", "confused" }, CONFIRM, [this](std::string e)
		{
			if (e == "angry")
			{
				SetEyes(&m_effectRunner, Now(), std::chrono::seconds(5), "eyes_angry", WHITE);
			}
			if (e == "heart")
			{
				SetEyes(&m_effectRunner, Now(), std::chrono::seconds(5), "eyes_heart", WHITE);
			}
			if (e == "cross")
			{
				SetEyes(&m_effectRunner, Now(), std::chrono::seconds(5), "eyes_cross", WHITE);
			}
			if (e == "confused")
			{
				SetEyes(&m_effectRunner, Now(), std::chrono::seconds(5), "eyes_confused", WHITE);
			}

			return "eyes set";
		});


}