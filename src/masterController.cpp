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
		m_loadedCommand = {};
		m_headset.AddSpeech("canceled command");
		return true;
		});

	std::string jsgfFilepath = "/home/pi/temp/pocketsphinx/test.gram";
	m_userControl.GenerateJSGF(jsgfFilepath);
	m_speechRecognition.SetJSGF(jsgfFilepath);

	m_recordButton.RegisterCallbackDown([this]() { return m_speechRecognition.Start(); });
	m_recordButton.RegisterCallbackUp([this]() { return m_speechRecognition.Stop(); });

	m_speechRecognition.RegisterCommandCallback([this](std::string command) {return LoadCommand(command); });
	m_confirmButton.RegisterCallbackDown([this]() {return RunCallback(); });

	m_headset.StartMainLoop();
	m_recordButton.StartMainLoop();
	m_confirmButton.StartMainLoop();
	return true;
}

bool TEF::Aurora::MasterController::RunCallback()
{
	if (!m_loadedCommand)
	{
		spdlog::error("no callback registered");
		m_headset.AddSpeech("No callback registered");
		return false;
	}

	if (m_loadedCommand->Run())
	{
		m_headset.AddSpeech("command success");
	}
	else
	{
		m_headset.AddSpeech("command failed");
	}

	m_loadedCommand = {};

	return true;
}

bool TEF::Aurora::MasterController::LoadCommand(std::string command)
{
	if (!m_userControl.FetchCommand(command, m_loadedCommand))
	{
		spdlog::error("Failed to fetch command");
		return false;
	}
	
	if (command == "cancel that")
	{
		return RunCallback();	
	}
	else
	{
		std::stringstream ss;
		ss << m_loadedCommand->GetCommand() << " " << m_loadedCommand->GetArg() << ", confirm?";
		m_headset.AddSpeech(ss);
	}
	
	return true;
}
