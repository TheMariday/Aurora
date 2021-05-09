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
	printf(cmd->command.c_str());
	printf("\n");
	cmd->status = 0;
	auto pPipe = ::popen(cmd->command.c_str(), "r");
	if (pPipe == nullptr)
	{
		throw std::runtime_error("Cannot open pipe");
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

void quickCommand(std::string command)
{
	CMD cmd;
	cmd.command = command;
	execCommand(&cmd);
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


bool TEF::Aurora::Sound::WaitFor(std::string speech)
{
	bool found = true;

	while (found)
	{
		found = std::find(m_speeches.begin(), m_speeches.end(), speech) != m_speeches.end();
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
	return true;
}

bool TEF::Aurora::Sound::WaitFor()
{
	while (!m_speeches.empty()) {
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}

	return true;
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

	quickCommand(SpeechToCommand(speech));

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
	quickCommand("pkill aplay");
	return true;
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

	quickCommand(command);
	return true;
}