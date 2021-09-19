#include "tef/aurora/masterController.h"
#include <spdlog/spdlog.h>

TEF::Aurora::MasterController::MasterController()
{
}

TEF::Aurora::MasterController::~MasterController()
{
}

bool TEF::Aurora::MasterController::Start()
{

	// Setup headset audio
	//m_headset.Connect("sysdefault:CARD=Device");

	// Setup tail bass
	//m_tailbass.Connect("I dunno");

	// Setup battery monitor, not sure if this should be done here as most of this is control stuff. might break out into a separate hardware class
	m_dac.Connect();
	m_batteryMonitor.Connect(&m_dac);
	m_batteryMonitor.RegisterErrorHandler([this](Error e) {Report(e); });

	// Setup SmartFuse

	if (!m_smartFuse.Connect())
	{
		spdlog::error("Master Controller cannot connect to smart fuse");
		return false;
	}

	m_smartFuse.RegisterErrorHandler([this](Error e) {Report(e); });

	// Setup record button

	if (!m_recordButton.Connect(&m_dac, Settings::PIN_RECORD))
	{
		spdlog::error("Master Controller cannot connect to record button");
		return false;
	};

	m_recordButton.RegisterCallbackDown([this]() {return m_speechRecognition.Start(); });

	m_recordButton.RegisterCallbackUp([this]() {return m_speechRecognition.Stop(); });

	// Set Record button error handling

	m_recordButton.RegisterErrorHandler([this](Error e) { Report(e); });

	// Setup confirm button

	if (!m_confirmButton.Connect(&m_dac, Settings::PIN_CONFIRM))
	{
		spdlog::error("Master Controller cannot connect to record button");
		return false;
	};

	m_confirmButton.RegisterCallbackDown([this]() {return RunCallback(m_loadedCommand); });

	// Set Confirm button error handling

	m_confirmButton.RegisterErrorHandler([this](Error e) { Report(e); });

	// Setup speech recognition

	if (!m_speechRecognition.Connect())
	{
		spdlog::error("Speech recognition failed to initialise");
		return false;
	};

	// output raw speech recognition snippet if we need to
	//m_speechRecognition.SetRecordFile("/home/pi/projects/Aurora/bin/ARM/Debug/raw.dat");

	// Setup all our lovely voice commands
	SetupVoiceCommands();

	// Set JSGF here. Last step!

	std::string jsgfFilepath = "/home/pi/temp/pocketsphinx/test.gram";
	m_userControl.GenerateJSGF(jsgfFilepath);
	m_speechRecognition.SetJSGF(jsgfFilepath);

	m_speechRecognition.RegisterCommandCallback([this](std::string command) {return LoadCommand(command); });

	// and lets kick everything off!

	m_smartFuse.Run();
	m_headset.Run();
	//m_tailbass.Run();
	m_recordButton.Run();
	m_confirmButton.Run();
	m_batteryMonitor.Run();

	return true;
}

TEF::Aurora::Sound* TEF::Aurora::MasterController::GetNotifier()
{
	if (m_headset.IsConnected())
		return &m_headset;
	
	if (m_tailbass.IsConnected())
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
		GetNotifier()->AddSpeech("Critical Shutdown in effect");
	}

	GetNotifier()->AddSpeech(e.str());

	return false;
}



bool TEF::Aurora::MasterController::SetupVoiceCommands()
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
		});

	m_userControl.RegisterVoid("fault", [this]()
		{
			return CriticalFault();
		});

	m_userControl.RegisterVoid("get fault status", [this]()
		{
			GetNotifier()->AddSpeech(m_fault ? "There is currently a fault" : "There are currently no faults");
			return true;
		});

	m_userControl.RegisterVoid("are you ready", [this]()
		{
			GetNotifier()->AddSpeech("I am ready, lets do this!");
			return true;
		});

	return true;
}

bool TEF::Aurora::MasterController::RunCallback(std::shared_ptr<Command> command)
{
	if (!command)
	{
		spdlog::warn("no callback to run");
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
		spdlog::error("Failed to fetch command");
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
