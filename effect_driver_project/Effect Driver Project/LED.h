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
		return { static_cast<int>(static_cast<float>(r) * f), static_cast<int>(static_cast<float>(g) * f), static_cast<int>(static_cast<float>(b) * f) };
	}
};

struct HSV
{
	HSV(float a, float b, float c) : h(a), s(b), v(c) {};
	float h = 0;
	float s = 0;
	float v = 0;
};

struct Loc
{
	Loc operator+(const Loc& b) const
	{
		return { x + b.x, y + b.y, z + b.z };
	}

	Loc& operator+=(const Loc& b) {
		x += b.x;
		y += b.y;
		z += b.z;
		return *this;
	}

	Loc& operator/=(const int& b) {
		x /= b;
		y /= b;
		z /= b;
		return *this;
	}

	Loc operator/(const float& b) const
	{
		return { static_cast<int>(x / b), static_cast<int>(y / b), static_cast<int>(z / b) };
	}

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
	LED(int i) { index = i; };

	int index;

	RGB rgb;

	void SetHSV(HSV hsv) { rgb = HSV2RGB(hsv); }
};