#pragma once
#include <thread>
#include <atomic>

namespace TEF::Aurora
{
	class Runnable
	{
	public:
		bool Run();
		void SetFPS(const float fps, bool ignoreOverrun = false);
		const float GetFPS();
		const float GetUtilisation() { return m_utilisation; }

	protected:
		virtual bool MainLoopCallback() = 0;
		Runnable();
		~Runnable();

	private:

		bool MainLoop();

		std::chrono::nanoseconds m_timeDeltaTarget;
		float m_utilisation = 0;

		std::atomic<bool> m_running;
		bool m_ignoreOverrun = false;

		std::thread m_mainLoopThread;
		std::chrono::high_resolution_clock::time_point m_lastMainloop;
	};
}
