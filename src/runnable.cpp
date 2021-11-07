#include "tef/aurora/runnable.h"
#include <spdlog/spdlog.h>
#include <chrono>
#include <thread>


TEF::Aurora::Runnable::Runnable()
{
	m_running = false;
}

TEF::Aurora::Runnable::~Runnable()
{
	m_running = false;
	if (m_mainLoopThread.joinable()) {
		m_mainLoopThread.join();
	}
}

bool TEF::Aurora::Runnable::MainLoopCallback()
{
	return true;
}

bool TEF::Aurora::Runnable::Report(Error error)
{
	error.log();
	if (m_errorHandler) m_errorHandler(error);
	return true;
}

bool TEF::Aurora::Runnable::IsConnected()
{
	return m_connected;
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
		m_utilisation = (float)(1 - (remainingCycles.count() / m_timeDeltaTarget.count()));

		if (m_utilisation > 1 and !m_ignoreOverrun)
		{
			spdlog::warn("{} over-utilisation!", typeid(*this).name());
		}

		std::this_thread::sleep_until(nextFrame);

	}
	return true;
}

void TEF::Aurora::Runnable::Run()
{
	m_running = true;

	m_timeDeltaTarget = std::chrono::nanoseconds((m_fps <= 0) ? 0 : int(1e+9 / m_fps));

	// if the thread is running we can't run it again
	if (m_mainLoopThread.joinable())
	{
		spdlog::error("{} cannot be Run twice", typeid(*this).name());
		return;
	}

	m_mainLoopThread = std::thread(&TEF::Aurora::Runnable::MainLoop, this);
}

void TEF::Aurora::Runnable::SetFPS(float fps, bool ignoreOverrun)
{
	m_ignoreOverrun = ignoreOverrun;

	m_fps = fps;

	spdlog::debug("Framerate set to {}", m_fps);
}

const float TEF::Aurora::Runnable::GetFPS()
{
	return float(double(1e+9) / (double)m_timeDeltaTarget.count());
}