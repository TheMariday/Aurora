#pragma once
#include "Texture.h"
#include "LedHarness.h"

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

		double t = static_cast<float>(radAngle / (M_PI * 2)) + m_offset;

		if (t < 0) t += static_cast<int>(t) + 1;

		double normAngle = abs(fmod(t, 1.0));
		if (m_flip)
			normAngle = 1 - normAngle;

		return { static_cast<float>(normAngle), 1.0f, 1.0f };
	}

	Loc m_center;
	float m_offset = 0;
	axis m_axis;
	bool m_flip = false;
};


class LinearRainbowTexture : public Texture
{
public:
	LinearRainbowTexture(Harness* harness, Loc center = { 0,0,0 }, axis ax = z_axis, bool flip = false, int scale = 1000) :
		Texture(harness), m_center(center), m_axis(ax), m_flip(flip), m_scale(scale){}

	HSV TextureLed(LED* led)
	{
		Loc loc = GetHarness()->GetLoc(led);

		double a = 0;

		switch (m_axis)
		{
		case x_axis:
			a = (loc.x - m_center.x) / m_scale;
			break;
		case y_axis:
			a = (loc.y - m_center.y) / m_scale;
			break;
		case z_axis:
			a = (loc.z - m_center.z) / m_scale;
			break;
		}

		double normAngle = fmod(a + m_offset, 1.0f);
		if (m_flip)
			normAngle = 1 - normAngle;

		return { static_cast<float>(normAngle), 1.0f, 1.0f };
	}

	Loc m_center;
	float m_offset = 0;
	axis m_axis;
	bool m_flip = false;
	float m_scale;
};


