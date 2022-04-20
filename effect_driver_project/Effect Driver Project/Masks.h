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
	GlowMask(Harness* harness, Loc center, int maxDistance) : Mask(harness)
	{
		m_center = center;
		m_maxDistance = maxDistance;
	}

	float GetAlpha(LED* led) override
	{
		int distance = Distance(m_center, GetHarness()->GetLoc(led));

		if (distance < m_maxDistance)
			return static_cast<float>(distance) / m_maxDistance;
		else
			return 0;
	}

private:
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
		if (a != 1.0f)
			int b;
		return a;
	}

	int m_center;
	axis m_axis;
	int m_width;
};


class OrbMask : public Mask
{
public:

	OrbMask(Harness* harness, Loc center, int diameter, bool inner = true) :
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
	RandomMask(Harness* harness, float prob) : Mask(harness), m_prob(prob)
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