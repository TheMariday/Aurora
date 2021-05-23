#include "tef/aurora/masterController.h"
#include <spdlog/spdlog.h>



TEF::Aurora::MasterController::MasterController() : m_recordButton(2), m_confirmButton(3), m_headset("sysdefault:CARD=Device")
{
}

TEF::Aurora::MasterController::~MasterController()
{
}

bool TEF::Aurora::MasterController::Start()
{
	m_speechRecognition.SetRecordFile("/home/pi/projects/Aurora/bin/ARM/Debug/raw.dat");

	m_userControl.RegisterVoid("cancel that", [this]() {
		std::stringstream ss;
		ss << "canceled command " << m_loadedCommand->GetCommandAndArgs();
		m_headset.AddSpeech(ss);
		m_loadedCommand = {};
		return true;
		}, false);

	m_userControl.RegisterVoid("whats loaded", [this]() {

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
		}, false);

	std::string jsgfFilepath = "/home/pi/temp/pocketsphinx/test.gram";
	m_userControl.GenerateJSGF(jsgfFilepath);
	m_speechRecognition.SetJSGF(jsgfFilepath);

	m_recordButton.RegisterCallbackDown([this]() { return m_speechRecognition.Start(); });
	m_recordButton.RegisterCallbackUp([this]() { return m_speechRecognition.Stop(); });

	m_speechRecognition.RegisterCommandCallback([this](std::string command) {return LoadCommand(command); });
	m_confirmButton.RegisterCallbackDown([this]() {return RunCallback(m_loadedCommand); });

	m_headset.Start();
	m_recordButton.Start();
	m_confirmButton.Start();
	return true;
}

bool TEF::Aurora::MasterController::RunCallback(Command* command)
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
	Command* command;
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
