#pragma once
#include "tef/aurora/effect.h"

namespace TEF::Aurora {

	class MasterController : public Effect {
	public:
		MasterController();
		~MasterController();
		bool MainLoopCallback() override;
		bool registerEffect(Effect* pEffect);
	private:
		std::vector<Effect*> m_effectVector;
		std::vector<LED> m_LEDs;
	};
};
