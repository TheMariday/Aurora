#pragma once
#include <thread>
#include <atomic>
#include "svl/SVL.h"

namespace TEF::Aurora
{
	class Effect
	{
	public:
		Effect();
		virtual bool Load();
		virtual bool Start();
		virtual bool Pause();
		virtual bool Stop();
		virtual bool Shader(Vec4& rgba, bool& metadata); // todo define metadata
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
