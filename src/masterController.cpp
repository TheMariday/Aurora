#include "tef/aurora/masterController.h"

#include <spdlog/spdlog.h>
#include <sstream>

#include "tef/aurora/properties.h"


TEF::Aurora::MasterController::MasterController()
{
}

TEF::Aurora::MasterController::~MasterController()
{
}

bool TEF::Aurora::MasterController::Start()
{
	if (TEF::Aurora::Properties::GetProperty<bool>("headset", "enabled").value_or(false))
	{
		if (!StartHeadset()) return false;
	}

	if (TEF::Aurora::Properties::GetProperty<bool>("tail", "enabled").value_or(false))
	{
		if (!StartTail()) return false;
	}

	if (TEF::Aurora::Properties::GetProperty<bool>("battery", "enabled").value_or(false))
	{
		if (!StartBattery()) return false;
	}

	if (TEF::Aurora::Properties::GetProperty<bool>("fuse", "enabled").value_or(false))
	{
		if (!StartFuse()) return false;
	}

	if (TEF::Aurora::Properties::GetProperty<bool>("buttons", "enabled").value_or(false))
	{
		if (!StartButtons()) return false;
	}

	if (TEF::Aurora::Properties::GetProperty<bool>("speech", "enabled").value_or(false))
	{
		if (!StartSpeech()) return false;
	}

	GetNotifier()->AddSpeech("System started up");
	spdlog::info("Master controller started successfully");

	return true;
}

bool TEF::Aurora::MasterController::StartHeadset()
{
	spdlog::info("Master Controller starting headset");
	std::optional<std::string> card = Properties::GetProperty<std::string>("headset", "card");
	std::optional<std::string> device = Properties::GetProperty<std::string>("headset", "device");
	std::optional<std::string> volumeControl = Properties::GetProperty<std::string>("headset", "volume_control");
	if (card && device && volumeControl)
	{
		m_headset.Connect(card.value(), device.value(), volumeControl.value());
		m_headset.SetVolume(Properties::GetProperty<float>("headset", "volume_default").value_or(0.5f));
	}
	else
	{
		spdlog::error("Master Controller failed to start headset as properties file is missing headset parameters");
		return false;
	}

	m_headset.Run();

	spdlog::info("Master Controller started headset successfully");
	return true;
}

bool TEF::Aurora::MasterController::StartTail()
{
	spdlog::info("Master Controller starting tail");
	std::optional<std::string> card = Properties::GetProperty<std::string>("tail", "card");
	std::optional<std::string> device = Properties::GetProperty<std::string>("tail", "device");
	std::optional<std::string> volumeControl = Properties::GetProperty<std::string>("tail", "volume_control");
	if (card && device && volumeControl)
	{
		m_tailbass.Connect(card.value(), device.value(), volumeControl.value());
		m_headset.SetVolume(Properties::GetProperty<float>("headset", "volume_default").value_or(1.0f));
	}
	else
	{
		spdlog::error("Master Controller failed to start tail as properties file is missing headset parameters");
		return false;
	}

	m_tailbass.Run();
	
	spdlog::info("Master Controller started tail successfully");
	return true;
}

bool TEF::Aurora::MasterController::StartBattery()
{
	spdlog::info("Master Controller starting battery systems");
	if (!m_dac.Connect())
	{
		spdlog::error("Master Controller failed to start battery system as it cannot connect to the DAC");
		return false;
	}
	std::vector<int> batteryPins =
	{
		Properties::GetProperty<int>("battery", "pin_cell_0").value_or(0),
		Properties::GetProperty<int>("battery", "pin_cell_1").value_or(1),
		Properties::GetProperty<int>("battery", "pin_cell_2").value_or(2),
		Properties::GetProperty<int>("battery", "pin_cell_3").value_or(3),
	};

	float minVoltage = Properties::GetProperty<float>("battery", "voltage_cell_min").value_or(3.2f);

	if (!m_batteryMonitor.Connect(&m_dac, batteryPins, minVoltage))
	{
		spdlog::error("Master Controller failed to start battery system as battery monitor failed to connect");
		return false;
	}

	m_batteryMonitor.RegisterErrorHandler([this](Error e) {Report(e); });

	m_batteryMonitor.Run(Properties::GetProperty<float>("battery", "fps").value_or(1.0f));

	spdlog::info("Master Controller started battery system successfully");
	return true;
}

bool TEF::Aurora::MasterController::StartFuse()
{
	spdlog::info("Master Controller starting smart fuse systems");
	if (!m_smartFuse.Connect(Properties::GetProperty<std::string>("smartfuse", "device").value_or("/dev/ttyUSB0")))
	{
		spdlog::error("Master Controller failed to start smart fuse as it cannot connect");
		return false;
	}

	m_smartFuse.RegisterErrorHandler([this](Error e) {Report(e); });

	m_smartFuse.Run(Properties::GetProperty<float>("fuse", "fps").value_or(1.0f));

	spdlog::info("Master Controller started smart fuse system successfully");
	return true;
}

bool TEF::Aurora::MasterController::StartButtons()
{
	spdlog::info("Master Controller starting button systems");

	// Setup record button
	if (!m_recordButton.Connect(&m_dac, Properties::GetProperty<int>("buttons", "pin_record").value_or(6)))
	{
		spdlog::error("Master Controller failed to start buttons as record button cannot connect");
		return false;
	};

	m_recordButton.SetName("record");
	m_recordButton.RegisterErrorHandler([this](Error e) { Report(e); });

	m_recordButton.RegisterCallbackDown([this]() {return m_speechRecognition.Start(); });
	m_recordButton.RegisterCallbackUp([this]() {return m_speechRecognition.Stop(); });

	m_recordButton.Run(Properties::GetProperty<float>("buttons", "fps").value_or(100.0f));

	// Setup confirm button

	if (!m_confirmButton.Connect(&m_dac, Properties::GetProperty<int>("buttons", "pin_confirm").value_or(6)))
	{
		spdlog::error("Master Controller failed to start buttons as confirm button cannot connect");
		return false;
	};

	m_confirmButton.SetName("confirm");
	m_confirmButton.RegisterErrorHandler([this](Error e) { Report(e); });

	m_confirmButton.RegisterCallbackDown([this]() {return RunCallback(m_loadedCommand); });

	m_confirmButton.Run(Properties::GetProperty<float>("buttons", "fps").value_or(100.0f));

	spdlog::info("Master Controller started button system successfully");
	return true;
}

bool TEF::Aurora::MasterController::StartSpeech()
{
	spdlog::info("Master Controller starting speech recognition systems");

	std::optional<std::string> device = Properties::GetProperty<std::string>("speech", "device");

	if (!device)
	{
		spdlog::error("Master Controller failed to start speech as properties file is missing the speech device");
		return false;
	}

	if (!m_speechRecognition.Connect(device.value()))
	{
		spdlog::error("Master Controller failed to start speech recognition as it failed to connect");
		return false;
	};

	// output raw speech recognition snippet if we need to
	//m_speechRecognition.SetRecordFile("/home/pi/projects/Aurora/bin/ARM/Debug/raw.dat");

	// Setup all our lovely voice commands
	SetupVoiceCommands();

	// Set JSGF here. Last step!

	std::string jsgfFilepath = "/home/pi/temp/pocketsphinx/test.gram";
	m_userControl.GenerateJSGF(jsgfFilepath);
	if (!m_speechRecognition.SetJSGF(jsgfFilepath))
	{
		spdlog::error("Master Controller failed to start speech recognition as it failed to set the JSGF file");
		return false;
	}

	m_speechRecognition.RegisterCommandCallback([this](std::string command) {return LoadCommand(command); });

	spdlog::info("Master Controller started speech recognition system successfully");
	return true;
}

TEF::Aurora::Sound* TEF::Aurora::MasterController::GetNotifier()
{
	if (TEF::Aurora::Properties::GetProperty<bool>("headset", "enabled").value_or(false))
		return &m_headset;

	if (TEF::Aurora::Properties::GetProperty<bool>("headset", "enabled").value_or(false))
		return &m_tailbass;

	// fall back to external even if it's not connected
	spdlog::error("no audio device to send speech data to! sending it to the headset regardless");
	return &m_headset;

}

TEF::Aurora::UserControl* TEF::Aurora::MasterController::GetUserControl()
{
	return &m_userControl;
}

bool TEF::Aurora::MasterController::CriticalFault()
{
	m_fault = true;
	m_smartFuse.Disable();
	m_smartFuse.StopAll();
	m_effectRunner.Disable();
	m_effectRunner.StopAll();

	return true;
}

bool TEF::Aurora::MasterController::ClearFault()
{
	m_fault = false;
	m_effectRunner.Enable();
	m_smartFuse.Enable();
	return true;
}

bool TEF::Aurora::MasterController::Report(Error e)
{
	if (e.level == ErrorLevel::Critical)
	{
		CriticalFault();
		m_faultError = e;
		GetNotifier()->AddSpeech("Critical Shutdown in effect");
	}

	GetNotifier()->AddSpeech(m_faultError.str());

	return false;
}

void TEF::Aurora::MasterController::SetupVoiceCommands()
{

	m_userControl.RegisterVoid("cancel that", [this]()
		{
			std::stringstream ss;
			ss << "canceled command " << m_loadedCommand->GetCommandAndArgs();
			m_headset.AddSpeech(ss);
			m_loadedCommand = {};
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


	m_userControl.RegisterLimitedInt("set headset volume to ", [this](int v)
		{
			m_headset.SetVolume(static_cast<float>(v) / 10.0f);
			std::stringstream ss;
			ss << "headset volume set to " << v * 10 << " percent";
			GetNotifier()->AddSpeech(ss);
			return true;
		}, true);

	m_userControl.RegisterLimitedInt("set tail volume to ", [this](int v)
		{
			m_headset.SetVolume(static_cast<float>(v) / 10.0f);
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
			int volume = static_cast<int>(m_headset.GetVolume() * 100);

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
			m_tailbass.PlayAudio("~/eminem.wav");
			return true;
		}, true);
}

bool TEF::Aurora::MasterController::RunCallback(std::shared_ptr<Command> command)
{
	if (!command)
	{
		spdlog::warn("Master Controller cannot run callback as there are no callbacks to run");
		m_headset.AddSpeech("No callback registered");
		return false;
	}

	bool commandSuccess = command->Run();

	if (!commandSuccess)
	{
		std::stringstream ss;
		ss << command->GetCommandAndArgs() << " failed";
		m_headset.AddSpeech(ss);
		spdlog::error(ss.str());
		return false;
	}

	m_loadedCommand = {};

	return true;
}

bool TEF::Aurora::MasterController::LoadCommand(std::string commandStr)
{
	std::shared_ptr<Command> command;
	if (!m_userControl.FetchCommand(commandStr, command))
	{
		spdlog::error("Master Controller failed to fetch command");
		return false;
	}

	if (command->IsConfirmationRequired())
	{
		m_loadedCommand = command;
		std::stringstream ss;
		ss << m_loadedCommand->GetCommand() << " " << m_loadedCommand->GetArg() << ", confirm?";
		m_headset.AddSpeech(ss);
	}
	else
	{
		RunCallback(command);
	}

	return true;
}
