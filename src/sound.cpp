#include "tef/aurora/sound.h"
#include "tef/aurora/cmd.h"

#include <spdlog/spdlog.h>
#include <algorithm>
#include <sstream>
#include <mutex>

TEF::Aurora::Sound::Sound()
{
	SetFPS(0);
}

bool TEF::Aurora::Sound::Connect(std::string device)
{
	m_device = device;

	// need to add an "aplay - L" check here to make sure the device is connected
	return true;
}

bool TEF::Aurora::Sound::IsConnected()
{
	return !m_device.empty();
}

TEF::Aurora::Sound::~Sound()
{

}

bool TEF::Aurora::Sound::AddSpeech(const std::string speech, bool wait)
{
	spdlog::debug("Adding {}blocking speech: {}", wait ? "" : "non", speech);
	{
		std::scoped_lock lock(m_speechesMutex);
		m_speeches.push_back(speech);
	}

	if (wait) WaitFor(speech);

	return true;
}

bool TEF::Aurora::Sound::AddSpeech(std::stringstream& speech, bool wait)
{
	return AddSpeech(speech.str(), wait);
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

	if (!IsConnected())
	{
		spdlog::warn("Audio is not connected, using logger: ", speech);
		return true;
	}

	if (!TEF::Aurora::CMD::quickCommand(SpeechToCommand(speech)))
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
	std::thread t(TEF::Aurora::CMD::quickCommand, command);
	t.detach();

	return true;
}

bool TEF::Aurora::Sound::StopAudio(std::string filename)
{
	return StopCommand(AudioToCommand(filename));
}

bool TEF::Aurora::Sound::Stop()
{
	return TEF::Aurora::CMD::quickCommand("pkill aplay");
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

	return TEF::Aurora::CMD::quickCommand(command);
}