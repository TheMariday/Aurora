#pragma once
#include <thread>
#include <atomic>
#include <vector>
#include "svl/SVL.h"

namespace TEF::Aurora
{
	struct LED {
		Vec3 position;

		bool isValid() {
			return position != Vec3(0, 0, 0);
		}
	};

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

		void SetFPS(const float fps);
		const float GetFPS();

		const float GetUtilisation() { return m_utilisation; }

	protected:
		~Effect();
		virtual bool MainLoop();

		std::chrono::nanoseconds m_timeDeltaTarget;
		float m_utilisation = 0;
		std::atomic<bool> m_running;
	private:
		std::thread m_mainLoopThread;
		std::chrono::high_resolution_clock::time_point m_lastMainloop;
	};

};
