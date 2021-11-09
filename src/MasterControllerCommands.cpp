#include "tef/aurora/masterController.h"

void TEF::Aurora::MasterController::SetupVoiceCommands()
{

	m_userControl.RegisterVoid("cancel that", [this]()
		{
			std::stringstream ss;
			ss << "canceled command " << m_loadedCommand->GetCommandAndArgs();
			m_headset.AddSpeech(ss);
			m_loadedCommand.reset();
			return true;
		}, false);

	m_userControl.RegisterVoid("whats loaded", [this]()
		{
			std::stringstream ss;
			if (m_loadedCommand)
				ss << "Loaded command " << m_loadedCommand->GetCommandAndArgs();
			else
				ss << "No command loaded";

			m_headset.AddSpeech(ss);
			return true;
		}, false);

	m_userControl.RegisterVoid("clear fault", [this]()
		{
			return ClearFault();
		}, true);

	m_userControl.RegisterVoid("fault", [this]()
		{
			return CriticalFault();
		}, false);

	m_userControl.RegisterVoid("get active fault", [this]()
		{
			if (m_fault)
			{
				GetNotifier()->AddSpeech("There is currently a fault");
				GetNotifier()->AddSpeech(m_faultError.str());
			}
			else
			{
				GetNotifier()->AddSpeech("There are no faults");
			}

			return true;
		}, false);

	m_userControl.RegisterVoid("are you ready", [this]()
		{
			GetNotifier()->AddSpeech("I am ready, lets do this!");
			return true;
		}, false);

	m_userControl.RegisterVoid("get cell voltages", [this]()
		{
			std::vector<Cell> cells;
			m_batteryMonitor.GetCells(cells);
			std::stringstream ss;
			for (auto cell : cells)
				ss << "Cell " << cell.cellIndex << " at " << std::to_string(cell.currentVoltage).substr(0, 4) << " volts. ";

			GetNotifier()->AddSpeech(ss.str());
			return true;
		}, false);

	m_userControl.RegisterLimitedInt("set headset volume to", [this](int v)
		{
			m_headset.SetVolume(static_cast<float>(v) / 10.0f);
			std::stringstream ss;
			ss << "headset volume set to " << v * 10 << " percent";
			GetNotifier()->AddSpeech(ss);
			return true;
		}, true);

	m_userControl.RegisterLimitedInt("set tail volume to", [this](int v)
		{
			m_tailbass.SetVolume(static_cast<float>(v) / 10.0f);
			std::stringstream ss;
			ss << "tail volume set to " << v * 10 << " percent";
			GetNotifier()->AddSpeech(ss);
			return true;
		}, true);

	m_userControl.RegisterVoid("get headset volume", [this]()
		{
			int volume = static_cast<int>(m_headset.GetVolume() * 100);

			std::stringstream ss;
			ss << "headset volume at " << volume << " percent";
			GetNotifier()->AddSpeech(ss);
			return true;
		}, false);

	m_userControl.RegisterVoid("get tail volume", [this]()
		{
			int volume = static_cast<int>(m_tailbass.GetVolume() * 100);

			std::stringstream ss;
			ss << "tail volume at " << volume << " percent";
			GetNotifier()->AddSpeech(ss);
			return true;
		}, false);

	m_userControl.RegisterVoid("stop tail audio", [this]()
		{
			m_tailbass.Stop();
			return true;
		}, false);

	m_userControl.RegisterVoid("play eminem", [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/test/eminem.wav");
			return true;
		}, true);

	m_userControl.RegisterVoid("play speaker test", [this]()
		{
			m_tailbass.PlayAudio("/home/pi/media/test/thx.wav");
			return true;
		}, true);

	m_userControl.RegisterVoid("system shutdown", [this]()
		{
			m_quit = true;
			return true;
		}, true);

}