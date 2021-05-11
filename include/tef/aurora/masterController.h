#pragma once
#include "tef/aurora/effect.h"
#include "tef/aurora/sound.h"

namespace TEF::Aurora {

	class MasterController : public Effect {
	public:
		MasterController();
		~MasterController();
		bool MainLoopCallback() override;
		bool registerEffect(Effect* pEffect);

		bool Notify(std::string message);

		Sound* GetInternalSound();
		Sound* GetExternalSound();
	private:

		Sound m_externalSound;
		Sound m_internalSound;

		bool forwardAudio = true;

		std::vector<Effect*> m_effectVector;
		std::vector<LED> m_LEDs;

	};
};
