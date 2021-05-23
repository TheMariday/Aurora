#include "tef/aurora/effect.h"
#include <stdio.h>
#include <chrono>
#include <thread>
#include "spdlog/spdlog.h"

TEF::Aurora::Effect::Effect()
{
	spdlog::debug("Base Effect created!");
	m_running = false;
	SetFPS(1);
}

bool TEF::Aurora::Effect::RegisterMC(MasterController* mc)
{
	if (!mc) 
	{
		spdlog::error("Base Effect cannot register null master controller");
		return false;
	}

	m_pMC = mc;
	return true;
}

TEF::Aurora::Effect::~Effect()
{
	m_running = false;
	if (m_mainLoopThread.joinable()) {
		m_mainLoopThread.join();
	}

	spdlog::debug("Base Effect destroyed");
}


bool TEF::Aurora::Effect::MainLoopCallback()
{
	return true;
}

bool TEF::Aurora::Effect::MainLoop()
{
	while (m_running)
	{
		m_lastMainloop = std::chrono::high_resolution_clock::now();

		MainLoopCallback();

		if (m_timeDeltaTarget == std::chrono::nanoseconds(0)) continue;

		std::chrono::high_resolution_clock::time_point nextFrame = m_lastMainloop + m_timeDeltaTarget;

		std::chrono::high_resolution_clock::duration remainingCycles = nextFrame - std::chrono::high_resolution_clock::now();
		m_utilisation = 1 - (remainingCycles.count() / float(m_timeDeltaTarget.count()));

		if (m_utilisation > 1 and !m_ignoreOverrun)
		{
			spdlog::warn("{} over-utilisation!", typeid(*this).name());
		}

		std::this_thread::sleep_until(nextFrame);

	}
}

bool TEF::Aurora::Effect::Start()
{
	m_running = true;
	m_mainLoopThread = std::thread(&TEF::Aurora::Effect::MainLoop, this);
	return true;
}

void TEF::Aurora::Effect::SetFPS(float fps, bool ignoreOverrun)
{
	m_ignoreOverrun = ignoreOverrun;

	m_timeDeltaTarget = std::chrono::nanoseconds(0);

	if (fps > 0) {
		m_timeDeltaTarget = std::chrono::nanoseconds(int(1e+9 / fps));
	}
	spdlog::debug("Framerate set to {}ns", m_timeDeltaTarget.count());
}

const float TEF::Aurora::Effect::GetFPS()
{
	return double(1e+9) / m_timeDeltaTarget.count();
}