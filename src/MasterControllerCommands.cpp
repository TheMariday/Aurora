#include "tef/aurora/masterController.h"
#include "tef/aurora/effects/rainbowEffect.h"

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

	m_userControl.RegisterVoid("system shutdown", CONFIRM, [this]()
		{
			m_quit = true;
			return "";
		});


	m_userControl.RegisterVoid("start rainbow", CONFIRM, [this]()
		{
			std::shared_ptr<TEF::Aurora::Effect> rainbowEffect = std::make_shared<TEF::Aurora::Effects::RainbowEffect>();

			m_effectRunner.AddEffect("rainbow", rainbowEffect);

			rainbowEffect->Start();

			return "rainbow effect started";
		});

	m_userControl.RegisterVoid("stop rainbow", CONFIRM, [this]()
		{
			std::shared_ptr<TEF::Aurora::Effect> rainbowEffect;

			if (!m_effectRunner.GetEffect("rainbow", rainbowEffect))
				return "failed to find rainbow effect, has it been added?";

			rainbowEffect->Stop();

			m_effectRunner.RemoveEffect("rainbow");

			return "rainbow effect stopped";
		});

	m_userControl.RegisterBool("fuse safety", CONFIRM, [this](bool enabled)
		{
			// temporary adition to enable all channels until i think up something better
			for (unsigned int channel = 1; channel <= 5; ++channel)
				m_smartFuse.SetFet(channel, !enabled);

			return enabled ? "fuse safety enabled" : "fuse safety disabled";
		});
}