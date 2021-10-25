#pragma once
#include <vector>
#include <fadecandy/opc_client.h>

#include "tef/aurora/runnable.h"
#include "tef/aurora/effect.h"


namespace TEF::Aurora {


	class EffectRunner : public TEF::Aurora::Runnable
	{
	public:
		EffectRunner();
		~EffectRunner();

		bool Connect(std::string address);

		void Black();

		void Enable() { m_enabled = true; };
		bool Disable();
		bool StopAll();

		bool AddEffect(std::shared_ptr<Effect> effect);


	private:
		bool MainLoopCallback() override;

		bool WriteToFC();


	private:
		std::vector<LED> m_leds;
		std::vector<std::shared_ptr<Effect>> m_effects;

		OPCClient m_opc;
		std::vector<uint8_t> m_frameBuffer;

		bool m_enabled = true;

		const int m_ledCount = 512; // this needs changing
	};

};
