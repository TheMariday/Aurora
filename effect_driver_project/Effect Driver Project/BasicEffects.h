#pragma once
#include "Effect.h"
#include "Mask.h"
#include "Metronome.h"
#include "Easer.h"
#include "Texture.h"


class Twinkle : public Effect
{
public:
	Twinkle(Harness* harness, timestamp start, duration dur, HSV hsv, float prob, std::string group = "main") : Effect(harness, start, dur)
	{
		m_hsv = hsv;
		m_group = group;
		m_prob = prob;
	}

	void Render(timestamp t) override
	{
		int intProb = static_cast<int>(m_prob * RAND_MAX);

		for (LED* pLED : GetHarness()->GetGroup(m_group))
			if(rand() < m_prob)
				pLED->hsv = m_hsv;
		Stop();
	}
private:
	HSV m_hsv;
	std::string m_group;
	float m_prob;
};

class Flash : public Effect
{
public:
	Flash(Harness* harness, timestamp start, HSV hsv, int beat = -1, std::string group = "main") : Effect(harness, start)
	{
		m_hsv = hsv;
		m_group = group;
		m_beat = beat;
	}

	void Render(timestamp t) override
	{
		if (m_beat >= 0)
			std::cout << "beat: " << m_beat << std::endl;

		for (LED* pLED : GetHarness()->GetGroup(m_group))
			pLED->hsv = m_hsv;
		Stop();
	}
private:
	HSV m_hsv;
	std::string m_group;
	int m_beat;
};

class Solid : public Effect
{
public:
	Solid(Harness* harness, timestamp start, duration dur, HSV hsv, std::string group = "main") : Effect(harness, start, dur)
	{
		m_hsv = hsv;
		m_group = group;
	}

	void Render(timestamp t) override
	{
		for (LED* pLED : GetHarness()->GetGroup(m_group))
			pLED->hsv = m_hsv;
	}
private:
	HSV m_hsv;
	std::string m_group;
};

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

	void Render(timestamp t) override
	{
		for (LED* pLED : OrbMask(GetHarness(), GetHarness()->GetGroup("main"), m_center, m_diameter))
		{
			pLED->hsv = hsv;
		}
	}

	HSV hsv = { 1.0f, 1.0f, 1.0f };
	Loc m_center;
	int m_diameter;
};


class Orb : public Effect
{
public:
	Orb(Harness* harness, timestamp start, duration dur, Loc startLoc, Loc endLoc, HSV color, int diameter, bool fade = false) : Effect(harness, start, dur)
	{
		m_diameter = diameter;
		m_hsv = color;

		AddDriver(
			[this, diameter, startLoc, endLoc, start, dur](timestamp t) {
				Ease<int>(&m_center.x, t, startLoc.x, endLoc.x, start, dur);
				Ease<int>(&m_center.y, t, startLoc.y, endLoc.y, start, dur);
				Ease<int>(&m_center.z, t, startLoc.z, endLoc.z, start, dur);
			});
	};

	void Render(timestamp t) override
	{
		for (LED* pLED : OrbMask(GetHarness(), GetHarness()->GetGroup("main"), m_center, m_diameter))
		{
			pLED->hsv = m_hsv;
		}
	}

	HSV m_hsv;
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
				if (fade)
					Ease<float>(&this->m_hsv.v, t, 1, 0, start, dur, EaseType::LINEAR);

			});
	}

	void Render(timestamp t) override
	{

		for (LED* pLED : RingMask(GetHarness(), GetHarness()->GetGroup("main"), m_center, m_diameter, m_ring_width))
			pLED->hsv = m_hsv;
	}

	Loc m_center;
	int m_diameter = 0;
	int m_ring_width = 100;
	HSV m_hsv = { 1.0f, 1.0f, 1.0f };

private:
};

class RainbowSpin : public Effect
{
public:
	RainbowSpin(Harness* harness, Loc center, timestamp start, duration dur, axis ax, float cycles = 1.0f) : Effect(harness, start, dur)
	{
		m_center = center;
		m_axis = ax;
		AddDriver([this, start, dur, cycles](timestamp t) {
			Ease<float>(&this->m_offset, t, 0.0f, cycles, start, dur);
			});
	}

	void Render(timestamp t) override
	{
		Rainbow(GetHarness(), GetHarness()->GetGroup("main"), m_center, m_offset, m_axis);
	}

private:
	float m_offset = 0.0f;
	Loc m_center;
	axis m_axis;
};

class RainbowBallSpin : public Effect
{
public:
	RainbowBallSpin(Harness* harness, Loc center, timestamp start, duration dur, axis ax, float cycles = 1.0f, int ballDiameter = 300) : Effect(harness, start, dur)
	{
		m_center = center;
		m_axis = ax;
		m_ballDiameter = ballDiameter;

		AddDriver([this, start, dur, cycles](timestamp t) {
			Ease<float>(&this->m_offset, t, 0.0f, cycles, start, dur);
			});
	}

	void Render(timestamp t) override
	{
		std::vector<LED*> leds = OrbMask(GetHarness(), GetHarness()->GetGroup("main"), m_center, m_ballDiameter);
		Rainbow(GetHarness(), leds, m_center, m_offset, m_axis);
	}

private:
	float m_offset = 0.0f;
	Loc m_center;
	axis m_axis;
	int m_ballDiameter = 0;
};