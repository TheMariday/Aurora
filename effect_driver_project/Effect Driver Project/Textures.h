#pragma once
#include "Texture.h"

class SolidTexture : public Texture
{
public:
	SolidTexture(Harness* harness, HSV hsv = { 1.0f, 1.0f, 1.0f }) : Texture(harness), m_hsv(hsv) {}

	HSV TextureLed(LED* led) override { return m_hsv; }

	HSV m_hsv;

};

class RadialRainbowTexture : public Texture
{
public:
	RadialRainbowTexture(Harness* harness, Loc center = { 0,0,0 }, axis ax = z_axis, bool flip = false) :
		Texture(harness), m_center(center), m_axis(ax), m_flip(flip) {}

	HSV TextureLed(LED* led)
	{
		Loc loc = GetHarness()->GetLoc(led);

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

		double normAngle = fmod(static_cast<float>(radAngle / (M_PI * 2)) + m_offset, 1.0f);
		if (m_flip)
			normAngle = 1 - normAngle;

		led->hsv = { static_cast<float>(normAngle), 1.0f, 1.0f };
	}

	Loc m_center;
	float m_offset;
	axis m_axis;
	bool m_flip = false;
};