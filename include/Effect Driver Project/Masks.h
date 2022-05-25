#pragma once
#include "Mask.h"

class GroupMask : public Mask
{
public:
	GroupMask(Harness* harness, std::string group) : Mask(harness), m_group(group) {}

	float GetAlpha(LED* pLED) override
	{
		return (GetHarness()->InGroup(m_group, pLED)) ? 1.0f : 0.0f;
	}

	std::string m_group;
};


class GlowMask : public Mask
{
public:
	GlowMask(Harness* harness, Loc center, int maxDistance = 1000) : Mask(harness)
	{
		m_center = center;
		m_maxDistance = maxDistance;
	}

	float GetAlpha(LED* led) override
	{
		int distance = Distance(m_center, GetHarness()->GetLoc(led));

		if (distance < m_maxDistance)
			return 1.0f - (static_cast<float>(distance) / static_cast<float>(m_maxDistance));
		else
			return 0.0f;
	}

	Loc m_center;
	int m_maxDistance;
};

class BandMask : public Mask
{
public:
	BandMask(Harness* harness, int center, axis ax, int width) : Mask(harness), m_center(center), m_axis(ax), m_width(width)
	{
	}

	float GetAlpha(LED* pLED) override
	{
		Loc loc = GetHarness()->GetLoc(pLED);

		int distance = abs(loc[m_axis] - m_center);
		float a = (distance < m_width / 2) ? 1.0f : 0.0f;
		return a;
	}

	int m_center;
	axis m_axis;
	int m_width;
};


class OrbMask : public Mask
{
public:

	OrbMask(Harness* harness, Loc center, int diameter = 300, bool inner = true) :
		Mask(harness), m_diameter(diameter), m_center(center)
	{
	}

	float GetAlpha(LED* pLed)
	{
		if (m_diameter < 0) return 0.0f;

		Loc loc = GetHarness()->GetLoc(pLed);

		int distance = Distance(loc, m_center);

		return (distance < m_diameter / 2) ? 1.0f : 0.0f;
	}

	int m_diameter;
	Loc m_center;
};


class RingMask : public Mask
{
public:

	RingMask(Harness* harness, Loc center, int diameter, int ringWidth = 50) :
		Mask(harness), m_diameter(diameter), m_center(center), m_ringWidth(ringWidth)
	{
	}

	float GetAlpha(LED* pLed)
	{
		if (m_diameter < 0) return 0.0f;

		Loc loc = GetHarness()->GetLoc(pLed);

		int distance = Distance(loc, m_center);

		bool inRing = (distance < m_diameter) && (distance > (m_diameter - m_ringWidth));

		if (inRing)
			int a = 0;

		return inRing ? 1.0f : 0.0f;
	}

	int m_diameter;
	Loc m_center;
	int m_ringWidth;
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

class RandomMask : public Mask
{
public:
	RandomMask(Harness* harness, float prob = 0.5f) : Mask(harness), m_prob(prob)
	{
	}

	float GetAlpha(LED* pLED) override
	{
		int intProb = static_cast<int>(m_prob * 100);
		int prob = rand() % 100; // 0 - 99
		return (prob < intProb) ? 1.0f : 0.0f;
	}

	float m_prob;
};

class EmptyMask : public Mask
{
public:
	EmptyMask(Harness* harness) : Mask(harness)
	{
	}

	float GetAlpha(LED* pLED) override
	{
		return 0;
	}
};

class RadialMask : public Mask
{
public:
	RadialMask(Harness* harness, int bands = 5, Loc center = { 0,0,0 }, axis ax = z_axis, bool flip = true) :
		Mask(harness), m_center(center), m_axis(ax), m_flip(flip), m_bands(bands) {}

	float GetAlpha(LED* pLED) override
	{
		Loc loc = GetHarness()->GetLoc(pLED);

		double radAngle = 0;

		switch (m_axis)
		{
		case x_axis:
			radAngle = atan2(loc.y - m_center.y, loc.z - m_center.z);
			break;
		case y_axis:
			radAngle = atan2(loc.z - m_center.z, loc.x - m_center.x);
			break;
		case z_axis:
			radAngle = atan2(loc.y - m_center.y, loc.x - m_center.x);
			break;
		}

		double normAngle = fmod(static_cast<float>((radAngle * m_bands) / (M_PI * 2)) + m_offset, 1.0f);
		if (m_flip)
			normAngle = 1 - normAngle;

		return static_cast<float>(normAngle);
	}

	Loc m_center;
	float m_offset;
	axis m_axis;
	bool m_flip = false;
	int m_bands;
};

