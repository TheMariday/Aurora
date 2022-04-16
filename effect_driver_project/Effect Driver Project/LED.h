#pragma once
#include "hsv2rgb.h"

struct RGB
{
	int r = 0;
	int g = 0;
	int b = 0;

	int& operator[](int index)
	{
		switch (index) {
		case 0: return r;
		case 1: return g;
		case 2: return b;
		default: return r;
		}
	}

	RGB operator* (float f)
	{
		return { static_cast<int>(static_cast<float>(r) * f), static_cast<int>(static_cast<float>(g) * f), static_cast<int>(static_cast<float>(b) * f )};
	}
};

struct HSV
{
	float h = 0;
	float s = 0;
	float v = 0;
};

struct Loc
{
	int x = 0;
	int y = 0;
	int z = 0;
};

inline int Distance(const Loc& a, const Loc& b)
{
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

inline RGB HSV2RGB(HSV hsv)
{
	CHSV chsv;
	chsv.hue = hsv.h * 255;
	chsv.sat = hsv.s * 255;
	chsv.val = hsv.v * 255;
	CRGB crgb;
	hsv2rgb_rainbow(chsv, crgb);
	RGB rgb = { crgb.r, crgb.g, crgb.b };
	return rgb;
}

struct LED
{
	RGB rgb;
	Loc loc;

	bool mapped = false;
};