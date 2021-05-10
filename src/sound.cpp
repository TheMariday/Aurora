#include "tef/aurora/sound.h"
#include <algorithm>
#include <spdlog/spdlog.h>

struct CMD
{
	std::string command = "";
	std::string response = "";
	int status = 0;
};

bool execCommand(CMD* cmd)
{
	if (cmd->command.empty())
	{
		spdlog::error("Cannot call empty command");
		return false;
	}

	spdlog::debug("Command Executor: {}", cmd->command.c_str());
	cmd->status = 0;
	auto pPipe = ::popen(cmd->command.c_str(), "r");
	if (pPipe == nullptr)
	{
		spdlog::error("Command Executor cannot open pipe");
		return false;
	}

	std::array<char, 256> buffer;

	while (not std::feof(pPipe))
	{
		auto bytes = std::fread(buffer.data(), 1, buffer.size(), pPipe);
		cmd->response.append(buffer.data(), bytes);
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));

	auto rc = ::pclose(pPipe);

	if (WIFEXITED(rc))
	{
		cmd->status = WEXITSTATUS(rc);
	}

	return true;
}

bool quickCommand(std::string command)
{
	CMD cmd;
	cmd.command = command;
	return execCommand(&cmd);
}

TEF::Aurora::Sound::Sound(std::string device)
{
	m_device = device;
	SetFPS(0);
}

TEF::Aurora::Sound::~Sound()
{

}

bool TEF::Aurora::Sound::AddSpeech(std::string speech, bool wait)
{
	spdlog::debug("Adding {}blocking speech: {}", wait ? "" : "non", speech);
	{
		std::scoped_lock lock(m_speechesMutex);
		m_speeches.push_back(speech);
	}

	if (wait) WaitFor(speech);

	return true;
}

bool TEF::Aurora::Sound::RemoveSpeech(std::string speech)
{
	std::scoped_lock lock(m_speechesMutex);
	m_speeches.remove(speech);
	ForceSpeechStop(speech);
	return true;
}

bool TEF::Aurora::Sound::RemoveSpeech()
{
	std::scoped_lock lock(m_speechesMutex);
	for (std::string& speech : m_speeches)
	{
		ForceSpeechStop(speech);
	}
	m_speeches.clear();
	return true;
}

bool TEF::Aurora::Sound::InterruptSpeech(std::string speech)
{
	std::scoped_lock lock(m_speechesMutex);
	ForceSpeechStop(m_speeches.front());
	m_speeches.push_front(speech);
	return true;
}


void TEF::Aurora::Sound::WaitFor(std::string speech)
{
	bool found = true;

	while (found)
	{
		found = std::find(m_speeches.begin(), m_speeches.end(), speech) != m_speeches.end();
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
}

void TEF::Aurora::Sound::WaitFor()
{
	while (!m_speeches.empty()) {
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
}



bool TEF::Aurora::Sound::MainLoopCallback()
{
	std::string speech;

	{
		std::scoped_lock(m_speechesMutex);
		if (m_speeches.empty())
		{
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			return true;
		}

		speech = m_speeches.front();
	}

	if (!quickCommand(SpeechToCommand(speech)))
	{
		spdlog::error("Failed to speak");
		return false;
	}

	// There might be a bug here surrounding duplicate
	// speech strings and tiny concurrency issues but I CBA to fix it.

	if (m_speeches.empty()) return true;

	std::scoped_lock lock(m_speechesMutex);
	if (m_speeches.front() == speech)
		m_speeches.pop_front();

	return true;
}

bool TEF::Aurora::Sound::PlayAudio(std::string filename)
{
	std::string command = AudioToCommand(filename);
	std::thread t(quickCommand, command);
	t.detach();

	return true;
}

bool TEF::Aurora::Sound::StopAudio(std::string filename)
{
	return StopCommand(AudioToCommand(filename));
}

bool TEF::Aurora::Sound::Stop()
{
	return quickCommand("pkill aplay");
}

std::string TEF::Aurora::Sound::SpeechToCommand(std::string speech)
{
	std::string command = "espeak '" + speech + "' -z --stdout | aplay";

	if (!m_device.empty())
	{
		command += " -D " + m_device;
	}
	return command;
}

std::string TEF::Aurora::Sound::AudioToCommand(std::string audio)
{
	std::string command = "aplay";

	if (!m_device.empty())
	{
		command += " -D " + m_device;
	}
	command += " " + audio;
	return command;
}

bool TEF::Aurora::Sound::ForceSpeechStop(std::string speech)
{
	std::string command = "aplay";

	if (!m_device.empty())
	{
		command += " -D " + m_device;
	}

	return StopCommand(command);
}

bool TEF::Aurora::Sound::ForceAudioStop(std::string audio)
{
	return StopCommand(AudioToCommand(audio));
}

bool TEF::Aurora::Sound::StopCommand(std::string command)
{
	command = "pkill -f -x \"" + command + "\"";

	return quickCommand(command);
}