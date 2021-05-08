#include "tef/aurora/masterController.h"

TEF::Aurora::MasterController::MasterController() :
	m_externalSound(""), 
	m_internalSound("sysdefault:CARD=Audio")
{
	SetFPS(60);
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
	m_effectVector.push_back(pEffect);
	pEffect->RegisterMC(this);
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
