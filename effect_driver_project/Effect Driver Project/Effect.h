#pragma once

#include <chrono>
#include <thread>
#include <functional>
#include <vector>
#include "LedHarness.h"
#include "utils.h"
#include "Texture.h"
#include "Mask.h"

enum class EffectState
{
	READY,
	RUNNING,
	STOPPED
};


class Effect : public Drivable
{
public:
	Effect(Harness* harness, timestamp startTime, timestamp endTime) : 
		m_harness(harness), m_pMask(std::make_shared<Mask>(harness)), 
		m_pTexture(std::make_shared<Texture>(harness)), m_startTime(startTime), m_endTime(endTime)
	{
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
		m_pMask->UpdateDrivers(t);
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
		std::vector<std::pair<LED*, float>> leds = m_pMask->GetLEDs();

		for (std::pair<LED*, float> ledAlpha : leds)
		{
			LED* led = ledAlpha.first;
			float alpha = ledAlpha.second;

			HSV hsv = m_pTexture->TextureLed(led);

			led->hsv = MixHSV(hsv, led->hsv, alpha);
		}
	}

	virtual void postRender(timestamp t)
	{
		// none
	}

	void SetTexture(std::shared_ptr<Texture> pTexture) { m_pTexture = pTexture; };
	void SetMask(std::shared_ptr<Mask> pMask) { m_pMask = pMask; };
	std::shared_ptr<Mask> GetMask() { return m_pMask; };
	std::shared_ptr<Texture> GetTexture() { return m_pTexture; };

	void Stop() { m_state = EffectState::STOPPED; };
	bool HasStopped() { return m_state == EffectState::STOPPED; };
	timestamp GetStartTime() { return m_startTime; }
	Harness* GetHarness() { return m_harness; }

private:

	Harness* m_harness;
	std::shared_ptr<Mask> m_pMask;
	std::shared_ptr<Texture> m_pTexture;

	timestamp m_startTime;
	timestamp m_endTime;

	EffectState m_state = EffectState::READY;
};
