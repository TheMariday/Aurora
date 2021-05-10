#pragma once
#include "tef/aurora/effect.h"
#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

#include "tef/aurora/masterController.h"


namespace TEF::Aurora::Effects
{
	class SimpleEffect : public Effect{
	public:
		SimpleEffect()
		{
			SetFPS(1);
			spdlog::debug("Simple Effect created 2\n");
		};

		bool MainLoopCallback() override 
		{
			spdlog::debug("Simple Effect main loop looping 2\n");
			if(m_pMC)
				m_pMC->GetExternalSound()->AddSpeech("boop");
			std::this_thread::sleep_for(std::chrono::seconds(2));
			return true;
		}
	};
};
