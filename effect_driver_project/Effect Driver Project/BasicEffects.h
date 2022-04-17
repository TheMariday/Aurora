#pragma once
#include "Effect.h"
#include "Mask.h"
#include "Metronome.h"

class LocalisedFire : public Effect
{
public:
	LocalisedFire(Harness* harness, Loc center, int diameter = 120, int diameterNoise = 50, float hue = 0) : Effect(harness)
	{
		m_center = center;
		m_diameter = diameter;

		AddDriver(
			[this, diameter, diameterNoise, hue](timestamp t) {
				Cycle<int>(&this->m_diameter, t, diameter, diameter + diameterNoise, this->GetStartTime(), std::chrono::milliseconds(1), CycleType::RANDOM);
				Cycle<float>(&this->hsv.h, t, hue, hue + 0.1f, this->GetStartTime(), std::chrono::milliseconds(1), CycleType::RANDOM);
			});
	};

	void Render(Harness* harness, timestamp t) override
	{
		RGB rgb = HSV2RGB(hsv);
		for (LED* pLED : OrbMask(harness, harness->GetGroup("main"), m_center, m_diameter))
		{
			pLED->rgb = rgb;
		}
	}

	HSV hsv = { 1.0f, 1.0f, 1.0f };
	Loc m_center;
	int m_diameter;
};


class Ripple : public Effect
{
public:
	Ripple(Harness* harness, timestamp start, duration dur, Loc center, HSV color, int maxSize, int ringWidth, bool fade) : Effect(harness, start, dur)
	{
		m_center = center;
		m_hsv = color;
		m_ring_width = ringWidth;

		AddDriver(
			[this, start, dur, maxSize, fade](timestamp t) {
				Ease<int>(&m_diameter, t, 0, maxSize, start, dur, EaseType::LINEAR);
				Ease<int>(&m_diameter, t, -m_ring_width, maxSize - m_ring_width, start, dur, EaseType::LINEAR);
				if(fade)
					Ease<float>(&this->m_hsv.v, t, 1, 0, start, dur, EaseType::LINEAR);

			});
	}

	void Render(Harness* harness, timestamp t) override
	{
		RGB rgb = HSV2RGB(m_hsv);

		for (LED* pLED : RingMask(harness, harness->GetGroup("main"), m_center, m_diameter, m_ring_width))
			pLED->rgb = rgb;
	}

	Loc m_center;
	int m_diameter = 0;
	int m_ring_width = 100;
	HSV m_hsv = { 1.0f, 1.0f, 1.0f };

private:
};