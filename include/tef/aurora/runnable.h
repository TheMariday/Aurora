#pragma once
#include "tef/aurora/error.h"
#include <thread>
#include <atomic>

namespace TEF::Aurora
{
	class Runnable
	{
	public:
		void Run();

		void SetFPS(const float fps, bool ignoreOverrun = false);

		const float GetFPS();
		const float GetUtilisation() { return m_utilisation; }

		void RegisterErrorHandler(std::function<void(Error)> handler) { m_errorHandler = handler; };

		bool IsConnected();

	protected:
		Runnable();
		~Runnable();
		virtual bool MainLoopCallback();
		bool Report(Error e);
		void Stop();

		std::atomic<bool> m_connected = false;

	private:

		bool MainLoop();

		std::function<void(Error)> m_errorHandler;

		std::chrono::nanoseconds m_timeDeltaTarget;

		float m_fps = 1;

		float m_utilisation = 0;

		std::atomic<bool> m_running;

		bool m_ignoreOverrun = false;

		std::thread m_mainLoopThread;
		std::chrono::high_resolution_clock::time_point m_lastMainloop;
	};
}
