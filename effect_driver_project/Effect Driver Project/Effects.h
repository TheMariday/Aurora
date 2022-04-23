#pragma once
#include "Effect.h"
#include "Mask.h"
#include "Metronome.h"
#include "Easer.h"
#include "Texture.h"

#include "Masks.h"
#include "Textures.h"

class Ripple : public Effect
{
public:
	Ripple(Harness* harness, timestamp start, duration dur, Loc center, HSV color, int maxSize = 200, int ringWidth = 50, bool fade = true) :
		Effect(harness, start, start + dur)
	{
		auto ringMask = std::make_shared<RingMask>(harness, center, 0, ringWidth);

		ringMask->AddDriver([ringMask, maxSize, start, dur, fade, ringWidth](timestamp t) {
			Ease<int>(&ringMask->m_diameter, t, ringWidth, maxSize, start, dur, EaseType::LINEAR);
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

class BlackoutEffect : public Effect
{
public:
	BlackoutEffect(Harness* harness, timestamp start, duration dur) : Effect(harness, start, start + dur)
	{
	}

	void Render(timestamp t) override
	{
		std::vector<LED*> leds = GetHarness()->GetGroup("main");
		for (LED* led : leds)
		{
			led->hsv.v = 0;
		}
	}
};

class BoostEffect : public Effect
{
public:
	BoostEffect(Harness* harness, timestamp start, timestamp end, Metronome* tap) :
		Effect(harness, start, end), m_tap(tap)
	{
	}

	void Render(timestamp t) override
	{
		int beat = m_tap->GetBeat(t) / 2;
		if (beat != m_lastBeat)
		{
			std::vector<LED*> leds = GetHarness()->GetGroup("main");
			for (LED* led : leds)
			{
				led->hsv.s *= (1 - m_intensity);
			}
			m_lastBeat = beat;

		}

	}
private:
	int m_lastBeat = -1;
	Metronome* m_tap;
	float m_intensity = 0.4f;

};