#pragma once
#include <thread>
#include <atomic>
#include <vector>

namespace TEF::Aurora
{
	class MasterController;

	class Effect
	{
	public:
		Effect();
		virtual bool Start();
		virtual bool MainLoopCallback();

		void SetFPS(const float fps, bool ignoreOverrun = false);
		const float GetFPS();

		const float GetUtilisation() { return m_utilisation; }

		bool RegisterMC(MasterController* mc);

	protected:
		~Effect();
		MasterController* m_pMC;

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
