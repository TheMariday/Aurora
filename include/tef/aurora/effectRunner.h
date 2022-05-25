#pragma once
#include <map>
#include <fadecandy/opc_client.h>

#include "tef/aurora/runnable.h"
#include "Effect Driver Project/Effect.h"
#include "tef/aurora/smartFuse.h"

namespace TEF::Aurora {


	class EffectRunner : public TEF::Aurora::Runnable
	{
	public:
		EffectRunner();
		~EffectRunner();

		bool Connect(std::string address, SmartFuse* smartFuse = nullptr);

		void Enable() { m_enabled = true; };
		bool Disable();
		bool StopAll();

		bool AddEffect(std::shared_ptr<Effect> effect);

		void SetBrightness(float v);

		Harness m_harness;

	private:
		bool MainLoopCallback() override;

		bool WriteToFC();


	private:

		SmartFuse* m_smartFuse;
		
		LedBuffer m_ledBuffer;

		std::vector< std::shared_ptr<Effect> > m_effects;

		OPCClient m_opc;

		bool m_enabled = true;
	};

};
