#include "tef/aurora/masterController.h"
#include <spdlog/spdlog.h>

TEF::Aurora::MasterController::MasterController() :
	m_externalSound(""), 
	m_internalSound("sysdefault:CARD=Audio")
{
	SetFPS(60);
	m_externalSound.StartMainLoop();
	m_internalSound.StartMainLoop();
}

TEF::Aurora::MasterController::~MasterController()
{
}

bool TEF::Aurora::MasterController::MainLoopCallback()
{
	std::vector<Vec3> rgb(m_LEDs.size());
	for (Effect* effect : m_effectVector)
	{
		effect->Shader(rgb, m_LEDs);
	}
}

bool TEF::Aurora::MasterController::registerEffect(Effect* pEffect)
{
	if (!pEffect) {
		spdlog::error("Master Controller Cannot register null effect");
		return false;
	}

	m_effectVector.push_back(pEffect);

	if (!pEffect->RegisterMC(this))
	{
		spdlog::error("Master Controller failed to register itself with this effect\n");
		return false;
	}
	
	return true;
}

TEF::Aurora::Sound* TEF::Aurora::MasterController::GetInternalSound()
{
	return &m_internalSound;
}

TEF::Aurora::Sound* TEF::Aurora::MasterController::GetExternalSound()
{
	return &m_externalSound;
}
