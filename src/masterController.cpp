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
	m_batteryMonitor.SetLowBatteryCallback([this](Cell c)
		{
			std::stringstream ss;
			ss << "Cell " << c.cellIndex << "has dropped below acceptable levels";
			Report(Error(ErrorType::Battery, ErrorLevel::Warning, ss.str()));
		});

	// Setup SmartFuse

	if (!m_smartFuse.Connect())
	{
		spdlog::error("Master Controller cannot connect to smart fuse");
		return false;
	}

	m_smartFuse.RegisterDisconnect([this](int channel) 
		{
		std::stringstream ss;
		ss << "Subsystem " << channel << " has disconnected";
		Report(Error(ErrorType::Electrical, ErrorLevel::Critical, ss.str()));
		return true;
		});

	m_smartFuse.RegisterReconnect([this](int channel)
		{
			std::stringstream ss;
			ss << "Subsystem " << channel << " has reconnected";
			Report(Error(ErrorType::Electrical, ErrorLevel::Info, ss.str()));
			return true;
		});

	// Setup record button

	if (!m_recordButton.Connect(&m_dac, 6))
	{
		spdlog::error("Master Controller cannot connect to record button");
		return false;
	};

	m_recordButton.RegisterCallbackDown([this]() {return m_speechRecognition.Start(); });

	m_recordButton.RegisterCallbackUp([this]() {return m_speechRecognition.Stop(); });

	// Set Record button error handling

	m_recordButton.RegisterCallbackDisconnect([this]() {
		return Report(Error(ErrorType::Electrical, ErrorLevel::Critical, "Record button has disconnected"));
		});

	m_recordButton.RegisterCallbackReconnect([this]() {
		return Report(Error(ErrorType::Electrical, ErrorLevel::Warning, "Record button has reconnected"));
		});

	// Setup confirm button

	if (!m_confirmButton.Connect(&m_dac, 7))
	{
		spdlog::error("Master Controller cannot connect to record button");
		return false;
	};

	m_confirmButton.RegisterCallbackDown([this]() {return RunCallback(m_loadedCommand); });

	// Set Confirm button error handling

	m_confirmButton.RegisterCallbackDisconnect([this]() {
		return Report(Error(ErrorType::Electrical, ErrorLevel::Critical, "Confirm button has disconnected"));
		});

	m_confirmButton.RegisterCallbackReconnect([this]() {
		return Report(Error(ErrorType::Electrical, ErrorLevel::Warning, "Confirm button has reconnected"));
		});

	// Setup speech recognition

	if (!m_speechRecognition.Connect())
	{
		Report(Error(ErrorType::System, ErrorLevel::Critical, "Speech recognition failed to initialise"));
		return false;
	};

	m_speechRecognition.SetRecordFile("/home/pi/projects/Aurora/bin/ARM/Debug/raw.dat");

	m_userControl.RegisterVoid("cancel that", [this]() {return CancelCommand(); }, false);
	m_userControl.RegisterVoid("whats loaded", [this]() {return WhatsLoaded(); }, false);


	// Set JSGF here. Last step!

	std::string jsgfFilepath = "/home/pi/temp/pocketsphinx/test.gram";
	m_userControl.GenerateJSGF(jsgfFilepath);
	m_speechRecognition.SetJSGF(jsgfFilepath);

	m_speechRecognition.RegisterCommandCallback([this](std::string command) {return LoadCommand(command); });

	// and lets kick everything off!

	m_smartFuse.Run();
	m_headset.Run();
	m_recordButton.Run();
	m_confirmButton.Run();
	m_batteryMonitor.Run();

	return true;
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
	e.log();

	if (e.level == ErrorLevel::Critical)
	{
		CriticalFault();
		GetSound()->AddSpeech("Critical Shutdown in effect");
	}

	GetSound()->AddSpeech(e.str());

	return false;
}

bool TEF::Aurora::MasterController::CancelCommand()
{
	std::stringstream ss;
	ss << "canceled command " << m_loadedCommand->GetCommandAndArgs();
	m_headset.AddSpeech(ss);
	m_loadedCommand = {};
	return true;
}

bool TEF::Aurora::MasterController::WhatsLoaded()
{
	std::stringstream ss;
	if (m_loadedCommand)
	{
		ss << "Loaded command " << m_loadedCommand->GetCommandAndArgs();
	}
	else
	{
		ss << "No command loaded";
	}

	m_headset.AddSpeech(ss);
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
