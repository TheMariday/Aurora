#include "tef/aurora/effect.h"
#include <stdio.h>
#include <chrono>
#include <thread>

TEF::Aurora::Effect::Effect()
{
	printf("Effect created!\n");
	m_running = false;
	SetFPS(1);
}

TEF::Aurora::Effect::~Effect()
{
	m_running = false;
	if (m_mainLoopThread.joinable()) {
		m_mainLoopThread.join();
	}

	printf("Effect destroyed\n");
}

bool TEF::Aurora::Effect::Load()
{
	printf("Effect loaded\n");
}

bool TEF::Aurora::Effect::Start()
{
	printf("Effect starting\n");
}

bool TEF::Aurora::Effect::Pause()
{
	printf("Effect paused\n");
}

bool TEF::Aurora::Effect::Stop()
{
	printf("Effect stopping\n");
}

bool TEF::Aurora::Effect::Shader(std::vector<Vec3>& rgb, std::vector<LED>& LEDs)
{
	return true;
}

bool TEF::Aurora::Effect::MainLoopCallback()
{
	printf("default mainloop callback\n");
	return true;
}

bool TEF::Aurora::Effect::MainLoop()
{
	while (m_running)
	{
		m_lastMainloop = std::chrono::high_resolution_clock::now();

		MainLoopCallback();

		if (m_timeDeltaTarget > std::chrono::nanoseconds(0)) {
			auto nextFrame = m_lastMainloop + m_timeDeltaTarget;

			auto remainingCycles = nextFrame - std::chrono::high_resolution_clock::now();
			m_utilisation = 1 - (remainingCycles.count() / float(m_timeDeltaTarget.count()));

			if (m_utilisation > 1)
			{
				printf("Warning! Over utilisation!\n");
			}

			std::this_thread::sleep_until(nextFrame);
		}
	}
}

bool TEF::Aurora::Effect::StartMainLoop()
{
	m_running = true;
	m_mainLoopThread = std::thread(&TEF::Aurora::Effect::MainLoop, this);
	return true;
}

void TEF::Aurora::Effect::SetFPS(float fps)
{
	int nanoDelta = 1e+9 / fps;
	if (fps == -1) {
		nanoDelta = 0;
	}
	m_timeDeltaTarget = std::chrono::nanoseconds(nanoDelta);
}

const float TEF::Aurora::Effect::GetFPS()
{
	return double(1e+9) / m_timeDeltaTarget.count();
}