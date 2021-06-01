#pragma once
#include <vector>
#include <fadecandy/opc_client.h>

#include "tef/aurora/runnable.h"
#include "tef/aurora/effect.h"


namespace TEF::Aurora {
	

	class EffectRunner : public TEF::Aurora::Runnable
	{
	public:
		explicit EffectRunner(std::string hostport = "localhost:7890");

		~EffectRunner();

		bool AddEffect(std::shared_ptr<Effect> effect);

		bool MainLoopCallback() override;

		bool WriteToFC();

	private:
		std::vector<LED> m_leds;
		std::vector<std::shared_ptr<Effect>> m_effects;

		OPCClient m_opc;
		std::vector<uint8_t> m_frameBuffer;

		const int m_ledCount = 512;

	};

};
