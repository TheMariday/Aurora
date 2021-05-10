#pragma once
#include <thread>
#include <atomic>
#include <vector>
#include "svl/SVL.h"

#include "tef/aurora/led.h"

namespace TEF::Aurora
{
	class MasterController;

	class Effect
	{
	public:
		Effect();
		virtual bool Load();
		virtual bool Start();
		virtual bool Pause();
		virtual bool Stop();
		virtual bool Shader(std::vector<Vec3>& rgb, std::vector<LED>& LEDs);
		virtual bool MainLoopCallback();

		bool StartMainLoop();

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
