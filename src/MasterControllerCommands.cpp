#include "tef/aurora/masterController.h"
#include "Effect Driver Project/FightSong.h"

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

	m_userControl.RegisterVoid("are you ready", INSTANT, [this]()
		{
			return "I am ready, lets do this!";
		});

	m_userControl.RegisterVoid("get cell voltages", INSTANT, [this]()
		{
			std::vector<Cell> cells;
			m_batteryMonitor.GetCells(cells);
			std::stringstream ss;
			for (auto cell : cells)
				ss << "Cell " << cell.cellIndex << " at " << std::to_string(cell.currentVoltage).substr(0, 4) << " volts. ";

			return ss.str();
		});

	m_userControl.RegisterLimitedInt("set headset volume to", CONFIRM, [this](int volume)
		{
			m_headset.SetVolume(static_cast<float>(volume) / 10.0f);
			std::stringstream ss;
			ss << "headset volume set to " << volume;
			return ss.str();
		});

	m_userControl.RegisterLimitedInt("set tail volume to", CONFIRM, [this](int volume)
		{
			m_tailbass.SetVolume(static_cast<float>(volume) / 10.0f);
			std::stringstream ss;
			ss << "tail volume set to " << volume;
			return ss.str();
		});

	m_userControl.RegisterVoid("get headset volume", INSTANT, [this]()
		{
			int volume = static_cast<int>(m_headset.GetVolume() * 10);

			std::stringstream ss;
			ss << "headset volume at " << volume;
			return ss.str();
		});

	m_userControl.RegisterVoid("get tail volume", INSTANT, [this]()
		{
			int volume = static_cast<int>(m_tailbass.GetVolume() * 10);

			std::stringstream ss;
			ss << "tail volume at " << volume;
			return ss.str();
		});

	m_userControl.RegisterVoid("stop tail audio", INSTANT, [this]()
		{
			m_tailbass.Stop();
			return "";
		});

	m_userControl.RegisterString("play", { "eminem", "test" }, CONFIRM, [this](std::string audioName)
		{
			std::string filename;

			if (audioName == "eminem")
				filename = "/home/pi/media/test/eminem.wav";

			if (audioName == "speaker test")
				filename = "/home/pi/media/test/thx.wav";

			m_tailbass.PlayAudio(filename);

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
			return "";
		});

	m_userControl.RegisterVoid("say violence", CONFIRM, [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/i_am_programmed_to_avoid_violence.wav");
			return "";
		});

	m_userControl.RegisterVoid("play joe joe", CONFIRM, [this]()
		{
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
			m_tailbass.PlayAudio("/home/pi/media/voice_lines/perimeter_breach.wav");
			return "";
		});

	m_userControl.RegisterVoid("system shutdown", CONFIRM, [this]()
		{
			m_quit = true;
			return "";
		});



	m_userControl.RegisterBool("fuse safety", CONFIRM, [this](bool enabled)
		{
			// temporary adition to enable all channels until i think up something better
			for (unsigned int channel = 0; channel < 8; ++channel)
				m_smartFuse.SetFet(channel, !enabled);

			return enabled ? "fuse safety enabled" : "fuse safety disabled";
		});

	m_userControl.RegisterVoid("fight song", CONFIRM, [this]()
		{
			timestamp now = Now();

			for (unsigned int channel = 0; channel < 8; ++channel)
				m_smartFuse.SetFet(channel, true);

			FightSong(&m_effectRunner, &m_effectRunner.m_harness, now);

			return "fight song enabled";
		});
}