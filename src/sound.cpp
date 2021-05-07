#include "tef/aurora/sound.h"
#include <mutex>

std::string execCommand(const std::string cmd, int& out_exitStatus)
{
	out_exitStatus = 0;
	auto pPipe = ::popen(cmd.c_str(), "r");
	if (pPipe == nullptr)
	{
		throw std::runtime_error("Cannot open pipe");
	}

	std::array<char, 256> buffer;

	std::string result;

	while (not std::feof(pPipe))
	{
		auto bytes = std::fread(buffer.data(), 1, buffer.size(), pPipe);
		result.append(buffer.data(), bytes);
	}

	auto rc = ::pclose(pPipe);

	if (WIFEXITED(rc))
	{
		out_exitStatus = WEXITSTATUS(rc);
	}

	return result;
}

TEF::Aurora::Sound::Sound(std::string device)
{
	SetFPS(0);
	m_device = device;
}

TEF::Aurora::Sound::~Sound()
{
}

bool TEF::Aurora::Sound::Say(std::string sentence)
{
	std::scoped_lock lock(m_queueMutex);

	m_sentenceQueue.push_back(sentence);
	return true;
}

bool TEF::Aurora::Sound::Play(std::string filename)
{
	return false;
}

bool TEF::Aurora::Sound::MainLoopCallback()
{
	if (!m_sentenceQueue.empty())
	{
		std::string sentence;
		{
			std::scoped_lock lock(m_queueMutex);
			sentence = m_sentenceQueue.front();
			m_sentenceQueue.pop_front();
		}

		if (!m_enabled) return true;

		std::string command = "espeak '" + sentence + "'";

		if (m_device != "") {
			command += " --stdout | aplay -D " + m_device;
		}

		int status;
		std::string res = execCommand(command, status);
		printf("command: %s\n", command.c_str());
		printf("response: %s\n", res.c_str());
		printf("status: %i\n", status);
	}
	else
	{
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}

	return true;
}
