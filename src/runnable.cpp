#include "tef/aurora/runnable.h"
#include <spdlog/spdlog.h>
#include <chrono>
#include <thread>


TEF::Aurora::Runnable::Runnable()
{
	m_running = false;
	SetFPS(0);
}

TEF::Aurora::Runnable::~Runnable()
{
	m_running = false;
	if (m_mainLoopThread.joinable()) {
		m_mainLoopThread.join();
	}

	spdlog::debug("Base Effect destroyed");
}

bool TEF::Aurora::Runnable::Report(Error error)
{
	error.log();
	if (m_errorHandler) m_errorHandler(error);
	return true;
}

bool TEF::Aurora::Runnable::MainLoop()
{
	while (m_running)
	{
		m_lastMainloop = std::chrono::high_resolution_clock::now();

		if (!MainLoopCallback())
			spdlog::error("Runnable mainloop returning false");

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

bool TEF::Aurora::Runnable::Run()
{
	m_running = true;
	m_mainLoopThread = std::thread(&TEF::Aurora::Runnable::MainLoop, this);
	return true;
}

void TEF::Aurora::Runnable::SetFPS(float fps, bool ignoreOverrun)
{
	m_ignoreOverrun = ignoreOverrun;

	m_timeDeltaTarget = std::chrono::nanoseconds(0);

	if (fps > 0) {
		m_timeDeltaTarget = std::chrono::nanoseconds(int(1e+9 / fps));
	}
	spdlog::debug("Framerate set to {}ns", m_timeDeltaTarget.count());
}

const float TEF::Aurora::Runnable::GetFPS()
{
	return double(1e+9) / m_timeDeltaTarget.count();
}