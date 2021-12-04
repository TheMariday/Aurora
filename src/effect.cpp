#include "tef/aurora/effect.h"

bool TEF::Aurora::Effect::Start()
{
	m_running = true;
	m_startedAt = std::chrono::high_resolution_clock::now();
	return true;
}

bool TEF::Aurora::Effect::Stop()
{
	m_running = false;
	return true;
}

float TEF::Aurora::Effect::SecondsSinceStart()
{
	if (!m_running)
		return 0;

	std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();

	return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startedAt).count()) / 1000.0f;
}

bool TEF::Aurora::Effect::IsRunning()
{
	return m_running;
}
