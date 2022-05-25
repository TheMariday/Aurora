#pragma once

#include <chrono>
#include <thread>
#include <functional>
#include <vector>
#include "LedHarness.h"
#include "utils.h"
#include "Texture.h"
#include "Mask.h"
#include <algorithm>

enum class EffectState
{
	READY,
	RUNNING,
	STOPPED
};

class Effect : public Drivable
{
public:
	Effect(Harness* harness, timestamp startTime, timestamp endTime, std::shared_ptr<Texture> texture = nullptr) :
		m_harness(harness), m_startTime(startTime), m_endTime(endTime)
	{
		m_pTexture = texture ? texture : std::make_shared<Texture>(harness);
	}

	void Update(timestamp t)
	{
		if (m_state == EffectState::STOPPED)
			return;

		if (t >= m_startTime)
			m_state = EffectState::RUNNING;

		if (t > m_endTime)
			m_state = EffectState::STOPPED;

		if (m_state != EffectState::RUNNING)
			return;

		UpdateDrivers(t);

		for (std::shared_ptr<Mask>& m : m_masks)
			m->UpdateDrivers(t);

		m_pTexture->UpdateDrivers(t);

		PreRender(t);
		Render(t);
		postRender(t);
	}

	virtual void PreRender(timestamp t)
	{
		// none
	}

	virtual void Render(timestamp t)
	{
		std::vector<LED*> leds = GetHarness()->GetGroup("main");

		for (LED* led : leds)
		{
			float alpha = 1;
			for (std::shared_ptr<Mask> mask : m_masks)
			{
				float maskAlpha = mask->GetModifiedAlpha(led);
				switch (mask->m_maskMix)
				{
				case(MaskMix::ADD):
					alpha += maskAlpha;
					break;
				case(MaskMix::MULTIPLY):
					alpha *= maskAlpha;
					break;
				}
			}
			led->hsv = MixHSV(m_pTexture->TextureLed(led), led->hsv, alpha);
		}
	}

	virtual void postRender(timestamp t)
	{
		// none
	}

	void SetTexture(std::shared_ptr<Texture> pTexture) { m_pTexture = pTexture; };
	void SetMask(std::shared_ptr<Mask> pMask) {
		m_masks.push_back(pMask);
	};
	void ClearMasks()
	{
		m_masks.clear();
	}
	std::shared_ptr<Mask> GetMask(int index = 0) { return m_masks.at(index); };
	std::shared_ptr<Texture> GetTexture() { return m_pTexture; };

	virtual void Stop() { m_state = EffectState::STOPPED; }; // this can be overridden for unstoppable effects XD
	bool HasStopped() { return m_state == EffectState::STOPPED; };
	timestamp GetStartTime() { return m_startTime; }
	timestamp GetEndTime() { return m_endTime; };
	Harness* GetHarness() { return m_harness; }


private:

	Harness* m_harness;
	std::vector<std::shared_ptr<Mask>> m_masks;
	std::shared_ptr<Texture> m_pTexture;

	timestamp m_startTime;
	timestamp m_endTime;

	EffectState m_state = EffectState::READY;

	std::vector<std::shared_ptr<Effect>> m_subEffects;
};


class EffectRunner
{
public:
	EffectRunner()
	{

	}

	void AddEffect(std::shared_ptr<Effect> e)
	{
		m_effects.push_back(e);
	}

	void Update(timestamp t)
	{
		for (auto e : m_effects)
			e->Update(t);

		//remove dead sub effects
		m_effects.erase(std::remove_if(m_effects.begin(), m_effects.end(), [](const std::shared_ptr<Effect>& x) {return x->HasStopped(); }), m_effects.end());

	}
private:
	std::vector<std::shared_ptr<Effect>> m_effects;

};

