#pragma once
#include "Effect.h"
#include "Metronome.h"
#include "Easer.h"
#include "Masks.h"
#include "Textures.h"
#include "colors.h"

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

class RainbowRipple : public Effect
{
public:
	RainbowRipple(Harness* harness, timestamp start, duration dur, Loc center, int maxSize = 200, int ringWidth = 50, bool fade = true) :
		Effect(harness, start, start + dur)
	{
		auto ringMask = std::make_shared<RingMask>(harness, center, 0, ringWidth);

		ringMask->AddDriver([ringMask, maxSize, start, dur, fade, ringWidth](timestamp t) {
			Ease<int>(&ringMask->m_diameter, t, ringWidth, maxSize, start, dur, EaseType::LINEAR);
			if (fade)
				Ease<float>(&ringMask->m_intensity, t, 1.0f, 0.0f, start, dur, EaseType::BEIZIER);
			});

		SetMask(ringMask);

		auto rainbowTexture = std::make_shared<RadialRainbowTexture>(harness, center, y_axis);

		SetTexture(rainbowTexture);
	}
private:
};


class GroupSolid : public Effect
{
public:
	GroupSolid(Harness* harness, timestamp start, duration dur, std::string group, HSV hsv = WHITE)
		: Effect(harness, start, start + dur)
	{
		SetTexture(std::make_shared<SolidTexture>(harness, hsv));
		SetMask(std::make_shared<GroupMask>(harness, group));
	}

	GroupSolid(Harness* harness, timestamp start, timestamp end, std::string group, HSV hsv = WHITE)
		: Effect(harness, start, end)
	{
		SetTexture(std::make_shared<SolidTexture>(harness, hsv));
		SetMask(std::make_shared<GroupMask>(harness, group));
	}

};

class RainbowSpin : public Effect
{
public:

	RainbowSpin(Harness* harness, Loc center, timestamp start, duration dur, axis ax, float cycles = 1.0f) : Effect(harness, start, start + dur)
	{
		RainbowSpin(harness, center, start, start + dur, ax, cycles);
	}

	RainbowSpin(Harness* harness, Loc center, timestamp start, timestamp end, axis ax, float cycles = 1.0f) : Effect(harness, start, end)
	{
		auto rainbowTexture = std::make_shared<RadialRainbowTexture>(harness, center, ax);

		rainbowTexture->AddDriver([rainbowTexture, start, end, cycles](timestamp t) {
			Ease<float>(&rainbowTexture->m_offset, t, 0.0f, -cycles, start, end);
			});

		SetTexture(rainbowTexture);
	}

private:
};

class DimmerEffect : public Effect
{
public:
	DimmerEffect(Harness* harness, timestamp start, duration dur, float dimmer = 0.0f) :
		Effect(harness, start, start + dur), m_dimmer(dimmer)
	{
	}

	void Render(timestamp t) override
	{
		std::vector<LED*> leds = GetHarness()->GetGroup("main");
		for (LED* led : leds)
		{
			led->hsv.v *= m_dimmer;
		}
	}

	float m_dimmer;
};


class EmptyEffect : public Effect
{
public:
	EmptyEffect(Harness* harness, timestamp start, timestamp end) :
		Effect(harness, start, end)
	{
		SetMask(std::make_shared<EmptyMask>(harness));
	}
};

class BoostEffect : public Effect
{
public:
	BoostEffect(Harness* harness, timestamp start, timestamp end, std::shared_ptr<Metronome> tap) :
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
	std::shared_ptr<Metronome> m_tap;
	float m_intensity = 0.4f;

};

class CalibrationEffect : public Effect
{
public:

	CalibrationEffect(Harness* harness) : Effect(harness, Now(), Now() + std::chrono::seconds(5))
	{
		auto xBandMask = std::make_shared<BandMask>(harness, -160, x_axis, 50);
		auto start = Now();
		auto end = start + std::chrono::seconds(5);
		xBandMask->AddDriver([xBandMask, harness, start, end](timestamp t) { Ease<int>(&xBandMask->m_center, t, -160, 160, start, end); });

		SetTexture(std::make_shared<SolidTexture>(harness, RED));
		SetMask(xBandMask);
	}

private:
};
