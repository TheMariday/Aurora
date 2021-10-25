#pragma once
#include "tef/aurora/error.h"
#include <thread>
#include <atomic>

namespace TEF::Aurora
{
	class Runnable
	{
	public:
		void Run(float fps = 0);
		
		const float GetFPS();
		const float GetUtilisation() { return m_utilisation; }

		void RegisterErrorHandler(std::function<void(Error)> handler) { m_errorHandler = handler; };


	protected:
		Runnable();
		~Runnable();
		virtual bool MainLoopCallback() = 0;
		bool Report(Error e);


	private:

		void SetFPS(const float fps, bool ignoreOverrun = false);

		bool MainLoop();

		std::function<void(Error)> m_errorHandler;

		std::chrono::nanoseconds m_timeDeltaTarget;
		float m_utilisation = 0;

		std::atomic<bool> m_running;
		bool m_ignoreOverrun = false;

		std::thread m_mainLoopThread;
		std::chrono::high_resolution_clock::time_point m_lastMainloop;
	};
}
