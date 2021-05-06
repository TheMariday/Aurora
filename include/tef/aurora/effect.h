#pragma once
#include <thread>
#include <atomic>
#include "../lib/svl/SVL.h"

namespace TEF::Aurora 
{
	class Effect 
	{
	public:
		Effect();
		~Effect();
		virtual bool Load();
		virtual bool Start();
		virtual bool Pause();
		virtual bool Stop();
		virtual bool Shader(Vec4& rgba, bool &metadata); // todo define metadata
		virtual bool MainLoopCallback();

		bool StartMainLoop();

	protected:
		virtual bool MainLoop();

		float m_fps = 60;
		std::atomic<bool> m_running;
	private:
		std::thread m_mainLoopThread;
	};

};
