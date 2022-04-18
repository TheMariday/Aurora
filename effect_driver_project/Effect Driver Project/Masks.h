#pragma once
#include "Mask.h"

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

