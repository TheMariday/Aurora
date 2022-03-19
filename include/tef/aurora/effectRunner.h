#pragma once
#include <map>
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

		bool AddEffect(std::string uid, std::shared_ptr<Effect> effect, int order = 100);
		bool GetEffect(std::string uid, std::shared_ptr<Effect>& effect);
		bool RemoveEffect(std::string uid);

	private:
		bool MainLoopCallback() override;

		bool WriteToFC();


	private:
		std::vector<LED> m_leds;

		// This allows us to store a kind of ordered map
		std::map<int, std::map<std::string, std::shared_ptr<Effect>>> m_effects;

		OPCClient m_opc;
		std::vector<uint8_t> m_frameBuffer;

		bool m_enabled = true;

		const int m_ledCount = 512*5; // this needs changing
	};

};
