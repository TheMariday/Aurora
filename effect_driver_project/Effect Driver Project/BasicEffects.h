#pragma once
#include "Effect.h"
#include "Mask.h"
#include "Metronome.h"

class LocalisedFire : public Effect
{
public:
	LocalisedFire(Loc center, int diameter = 120, int diameterNoise = 50, float hue = 0) : Effect()
	{
		orbMask.center = center;

		AddDriver(
			[this, diameter, diameterNoise, hue](timestamp t) {
				Cycle<int>(&this->orbMask.diameter, t, diameter, diameter + diameterNoise, this->GetStartTime(), std::chrono::milliseconds(1), CycleType::RANDOM);
				Cycle<float>(&this->hsv.h, t, hue, hue + 0.1f, this->GetStartTime(), std::chrono::milliseconds(1), CycleType::RANDOM);
			});
	};

	void Render(Harness& harness, timestamp t) override
	{
		RGB rgb = HSV2RGB(hsv);
		for (LED* pLED : orbMask.GetMask(harness.GetMap()))
		{
			pLED->rgb = rgb;
		}
	}

	OrbMask orbMask;
	HSV hsv = { 1.0f, 1.0f, 1.0f };
};


class Ripple : public Effect
{
public:
	Ripple(Loc center, timestamp start, duration end, float hue) : Effect()
	{
		outerOrb.center = center;
		innerOrb.center = center;
		hsv.h = hue;

		AddDriver(
			[this, start, end, hue](timestamp t) {
				Ease<int>(&this->outerOrb.diameter, t, 0, this->size, start, end, EaseType::LINEAR);
				Ease<int>(&this->innerOrb.diameter, t, -ring_width, this->size - ring_width, start, end, EaseType::LINEAR);
				Ease<float>(&this->brightness, t, 1, 0, start, end, EaseType::LINEAR);
				if(t > start + end) Stop();
			});
	};

	void Render(Harness& harness, timestamp t) override
	{
		RGB rgb = HSV2RGB(hsv);
		std::vector<LED*> innerMask = innerOrb.GetMask(harness.GetMap());
		for (LED* pLED : outerOrb.GetMask(harness.GetMap()))
		{
			bool inRing = std::find(innerMask.begin(), innerMask.end(), pLED) == innerMask.end();
			
			if(inRing)
				pLED->rgb = rgb * brightness;
		}
	}

	int diameter = 0;
	int ring_width = 100;
	int size = 1000;
	float brightness = 1.0f;
	OrbMask outerOrb;
	OrbMask innerOrb;
	HSV hsv = { 1.0f, 1.0f, 1.0f };
};