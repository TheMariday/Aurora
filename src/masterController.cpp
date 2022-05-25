#include "tef/aurora/masterController.h"

#include <spdlog/spdlog.h>
#include <sstream>

#include "tef/aurora/properties.h"
#include "tef/aurora/effects/rainbowEffect.h"
#include "Effect Driver Project/Eyes.h"


TEF::Aurora::MasterController::MasterController()
{
}

TEF::Aurora::MasterController::~MasterController()
{
}

bool TEF::Aurora::MasterController::Start(bool cliEnabled)
{
	// Setup all our lovely voice commands
	SetupVoiceCommands();

	if (TEF::Aurora::Properties::GetProperty<bool>("headset", "enabled").value_or(false))
	{
		if (!StartHeadset()) return false;
		m_headset.PlayAudio("/home/pi/media/cyclops/AI_engine_up.wav");
	}

	if (TEF::Aurora::Properties::GetProperty<bool>("tail", "enabled").value_or(false))
	{
		if (!StartTail()) return false;
	}

	std::this_thread::sleep_for(std::chrono::seconds(3));

	if (TEF::Aurora::Properties::GetProperty<bool>("buttons", "enabled").value_or(false))
	{
		if (!StartButtons())
		{
			m_headset.AddSpeech("Master controller failed to start button system");
			return false;
		}
	}

	if (TEF::Aurora::Properties::GetProperty<bool>("battery", "enabled").value_or(false))
	{
		if (!StartBattery())
		{
			m_headset.AddSpeech("Master controller failed to start battery system");
			return false;
		}
	}

	if (TEF::Aurora::Properties::GetProperty<bool>("fuse", "enabled").value_or(false))
	{
		if (!StartFuse())
		{
			m_headset.AddSpeech("Master controller failed to start fuse system");
			return false;
		}
	}

	if (TEF::Aurora::Properties::GetProperty<bool>("speech", "enabled").value_or(false))
	{
		if (!StartSpeech())
		{
			m_headset.AddSpeech("Master controller failed to start speech recognition system");
			return false;
		}
	}

	if (TEF::Aurora::Properties::GetProperty<bool>("fadecandy", "enabled").value_or(false))
	{
		if (!StartEffectController())
		{
			m_headset.AddSpeech("Master controller failed to start effect controller system");
			return false;
		}
	}

	if (cliEnabled)
		StartCLI();

	for (Runnable* runnable : m_connectedRunnable)
	{
		runnable->RegisterErrorHandler([this](Error e) {Report(e); });
		runnable->Run();
	}

	m_headset.PlayAudio("/home/pi/media/cyclops/AI_welcome.wav");
	std::this_thread::sleep_for(std::chrono::seconds(3));

	spdlog::info("Master controller started successfully");


	return true;
}

bool TEF::Aurora::MasterController::StartEffectController()
{

	int tries = 0;
	while ((m_effectRunner.Connect("localhost", &m_smartFuse) == false) && tries < 10)
	{
		spdlog::error("failed to connect to effect runner, retrying ({})", tries);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		++tries;
	}

	if (tries >= 10)
	{
		spdlog::error("failed to connect to effect runner");
		return false;
	}

	m_effectRunner.AddEffect(std::make_shared<Eyes>(&m_effectRunner.m_harness));

	m_connectedRunnable.emplace_back(&m_effectRunner);

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
		m_headset.SetVolume(Properties::GetProperty<float>("headset", "volume_default").value_or(1.0f));
	}
	else
	{
		spdlog::error("Master Controller failed to start headset as properties file is missing headset parameters");
		return false;
	}

	m_connectedRunnable.emplace_back(&m_headset);

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
		m_tailbass.SetVolume(Properties::GetProperty<float>("tail", "volume_default").value_or(0.5f));
	}
	else
	{
		spdlog::error("Master Controller failed to start tail as properties file is missing headset parameters");
		return false;
	}

	m_connectedRunnable.emplace_back(&m_tailbass);

	spdlog::info("Master Controller started tail successfully");
	return true;
}

bool TEF::Aurora::MasterController::StartBattery()
{
	spdlog::info("Master Controller starting battery systems");
	if (!m_dac.isConnected())
	{
		if (!m_dac.Connect())
		{
			spdlog::error("Master Controller failed to start battery system as it cannot connect to the DAC");
			return false;
		}
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

	m_batteryMonitor.SetFPS(Properties::GetProperty<float>("battery", "fps").value_or(1.0f));

	m_connectedRunnable.emplace_back(&m_batteryMonitor);

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

	m_smartFuse.SetFPS(Properties::GetProperty<float>("fuse", "fps").value_or(1.0f));

	m_connectedRunnable.emplace_back(&m_smartFuse);

	spdlog::info("Master Controller started smart fuse system successfully");
	return true;
}

bool TEF::Aurora::MasterController::StartButtons()
{
	spdlog::info("Master Controller starting button systems");

	if (!m_dac.isConnected())
	{
		if (!m_dac.Connect())
		{
			spdlog::error("Master Controller failed to start battery system as it cannot connect to the DAC");
			return false;
		}
	}

	// Setup record button
	if (!m_recordButton.Connect(&m_dac, Properties::GetProperty<int>("buttons", "pin_record").value_or(4), "Record"))
	{
		spdlog::error("Master Controller failed to start buttons as record button cannot connect");
		return false;
	};


	m_recordButton.RegisterCallbackDown([this]() {return m_speechRecognition.Start(); });
	m_recordButton.RegisterCallbackUp([this]() {return m_speechRecognition.Stop(); });

	m_recordButton.SetFPS(Properties::GetProperty<float>("buttons", "fps").value_or(100.0f));

	m_connectedRunnable.emplace_back(&m_recordButton);

	// Setup confirm button

	if (!m_confirmButton.Connect(&m_dac, Properties::GetProperty<int>("buttons", "pin_confirm").value_or(5), "Confirm"))
	{
		spdlog::error("Master Controller failed to start buttons as confirm button cannot connect");
		return false;
	};

	m_confirmButton.RegisterCallbackDown([this]() {return RunLoadedCallback(); });

	m_confirmButton.SetFPS(Properties::GetProperty<float>("buttons", "fps").value_or(100.0f));

	m_connectedRunnable.emplace_back(&m_confirmButton);

	spdlog::info("Master Controller started button system successfully");
	return true;
}

bool TEF::Aurora::MasterController::StartSpeech()
{
	spdlog::info("Master Controller starting speech recognition systems");

	std::optional<std::string> device = Properties::GetProperty<std::string>("speech", "device");

	if (!device.has_value())
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
	m_speechRecognition.SetRecordFile("/home/pi/speech_debug.dat");

	// Set JSGF here. Last step!

	std::string jsgfFilepath = "/home/pi/test.gram";
	m_userControl.GenerateJSGF(jsgfFilepath);
	if (!m_speechRecognition.SetJSGF(jsgfFilepath))
	{
		spdlog::error("Master Controller failed to start speech recognition as it failed to set the JSGF file");
		return false;
	}

	m_speechRecognition.RegisterCommandCallback([this](std::string command) {LoadCommand(command); });

	m_connectedRunnable.emplace_back(&m_speechRecognition);

	spdlog::info("Master Controller started speech recognition system successfully");
	return true;
}

bool TEF::Aurora::MasterController::StartCLI()
{
	m_cli.RegisterCommandCallback([this](std::string command) {LoadCommand(command); });
	m_cli.RegisterConfirm([this]() {return RunLoadedCallback(); });
	m_connectedRunnable.emplace_back(&m_cli);
	return true;
}

TEF::Aurora::Sound* TEF::Aurora::MasterController::GetNotifier()
{
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
	/* this is a bad idea
	if (e.level == ErrorLevel::Critical && !m_fault)
	{
		CriticalFault();
		m_faultError = e;
		GetNotifier()->AddSpeech("Critical Shutdown in effect");
		GetNotifier()->AddSpeech(m_faultError.str());
	}
	*/
	GetNotifier()->AddSpeech(e.str());

	return false;
}

bool TEF::Aurora::MasterController::RunLoadedCallback()
{
	if (!m_loadedCommand)
	{
		spdlog::warn("Master Controller cannot run callback as there are no callbacks to run");
		GetNotifier()->AddSpeech("No callback registered");
		return false;
	}

	std::string returnedString = m_loadedCommand->Run();

	GetNotifier()->AddSpeech(returnedString);

	if (m_loadedCommand->GetLaunchAction() != INSTANT_TRIGGER)
	{
		UnloadCommand();
	}

	return true;
}

bool TEF::Aurora::MasterController::LoadCommand(std::string commandStr)
{
	if (commandStr.empty())
	{
		GetNotifier()->AddSpeech("no voice detected");
		return false;
	}

	std::shared_ptr<Command> command;
	if (!m_userControl.FetchCommand(commandStr, command))
	{
		spdlog::error("Master Controller failed to fetch command");
		return false;
	}

	std::stringstream response;

	switch (command->GetLaunchAction())
	{
	case INSTANT:
	{
		response << command->Run();
	}
	break;
	case CONFIRM:
	{
		response << command->GetCommandAndArgs() << ", confirm?";
		m_loadedCommand = command;
	}
	break;
	case INSTANT_TRIGGER:
	{
		response << command->GetCommandAndArgs() << " ready.";
		m_loadedCommand = command;
	}
	break;
	}

	m_headset.AddSpeech(response);

	return true;
}

bool TEF::Aurora::MasterController::UnloadCommand()
{
	m_loadedCommand.reset();
	return true;
}

void TEF::Aurora::MasterController::Spin()
{
	while (!m_quit)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}