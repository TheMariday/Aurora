#pragma once
#include "Effect.h"
#include "Mask.h"
#include "Metronome.h"
#include "Easer.h"
#include "Texture.h"

#include "Masks.h"
#include "Textures.h"

class Twinkle : public Texture
{
public:
	Twinkle(Harness* harness, timestamp start, timestamp end, HSV hsv, float prob) : Texture(harness), m_hsv(hsv), m_prob(prob)
	{
	}

	HSV TextureLed(LED* pLED)
	{
		int intProb = static_cast<int>(m_prob * RAND_MAX);

		if (rand() < m_prob)
			pLED->hsv = m_hsv;
	}

private:
	HSV m_hsv;
	float m_prob;
};

class WipeMask : public Mask
{
public:
	WipeMask(Harness* harness, axis ax, int axis_pos, bool greater = true) :
		Mask(harness), m_axis(ax), m_axis_pos(axis_pos), m_greater(greater)
	{

	}

	float GetAlpha(LED* pLED) override
	{
		int p = GetHarness()->GetLoc(pLED)[m_axis];

		if (m_greater ? p > m_axis_pos : p < m_axis_pos)
			return 1.0f;
		else
			return 0.0f;
	}

private:
	axis m_axis;
	int m_axis_pos;
	bool m_greater;

};


class Ripple : public Effect
{
public:
	Ripple(Harness* harness, timestamp start, duration dur, Loc center, HSV color, int maxSize = 500, int ringWidth = 50, bool fade = true) :
		Effect(harness, start, start + dur)
	{
		auto ringMask = std::make_shared<RingMask>(harness, center, 0, ringWidth);

		ringMask->AddDriver([ringMask, maxSize, start, dur, fade](timestamp t) {
			Ease<int>(&ringMask->m_diameter, t, 0, maxSize, start, dur, EaseType::LINEAR);
			if (fade)
				Ease<float>(&ringMask->m_intensity, t, 1.0f, 0.0f, start, dur, EaseType::BEIZIER);
			});

		SetMask(ringMask);

		auto solidColor = std::make_shared<SolidTexture>(harness, color);

		SetTexture(solidColor);
	}
private:
};

class RainbowSpin : public Effect
{
public:
	RainbowSpin(Harness* harness, Loc center, timestamp start, duration dur, axis ax, float cycles = 1.0f) : Effect(harness, start, start + dur)
	{
		auto rainbowTexture = std::make_shared<RadialRainbowTexture>(harness, center, ax);

		rainbowTexture->AddDriver([rainbowTexture, start, dur, cycles](timestamp t) {
			Ease<float>(&rainbowTexture->m_offset, t, 0.0f, cycles, start, dur);
			});

		SetTexture(rainbowTexture);
	}

private:
};