#pragma once
#include "hsv2rgb.h"
#include "cmath"
//#include "utils.h"

template <class T>
T limits(T v, T v1, T v2)
{
	v = std::min(v, (v1 < v2) ? v2 : v1);
	v = std::max(v, (v1 < v2) ? v1 : v2);
	return v;
}

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
	HSV() = default;
	HSV(float a, float b, float c) : h(a), s(b), v(c) {};

	HSV operator*(const float& b) const
	{
		return { h * b, s * b,v * b };
	}

	HSV operator+(const HSV& b) const
	{
		return { h + b.h, s + b.s, v + b.v };
	}

	float h = 0;
	float s = 0;
	float v = 0;
};

// this is hot garbage. do not touch
inline HSV MixHSV(HSV a, HSV b, float alpha)
{

	if (alpha >= 1.0f) return a;
	if (alpha <= 0.0f) return b;

	if (a.h - b.h > 0.5f) a.h -= 1.0f;

	HSV hsv;
	hsv.v = (alpha * a.v + (1 - alpha) * b.v);
	hsv.s = (alpha * a.s + (1 - alpha) * b.s);

	float aImportance = (a.s * a.v) * alpha;
	float bImportance = (b.s * b.v) * (1 - alpha);

	float denom = (aImportance + bImportance);

	// doesn't matter
	if (denom == 0)
	{
		hsv.h = 1.0f;
	}
	else
	{
		float ratio = aImportance / denom;
		hsv.h = a.h * ratio + b.h * (1 - ratio);
	}

	hsv.h = fmodf(hsv.h, 1.0f);

	return  hsv;

}

struct Loc
{
	Loc() : x(0), y(0), z(0) {};
	Loc(int a, int b, int c) : x(a), y(b), z(c) {};


	int& operator[](int index)
	{
		switch (index) {
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default: return z;
		}
	}

	Loc operator+(const Loc& b) const
	{
		return { x + b.x, y + b.y, z + b.z };
	}

	Loc operator-(const Loc& b) const
	{
		return { x - b.x, y - b.y, z - b.z };
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

	Loc operator*(const float& b) const
	{
		return { static_cast<int>(x * b), static_cast<int>(y * b), static_cast<int>(z * b) };
	}

	int x = 0;
	int y = 0;
	int z = 0;
};

inline int Distance(const Loc& a, const Loc& b)
{
	return static_cast<int>(sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2)));
}

inline RGB HSV2RGB(HSV hsv, float brightness)
{
	hsv.s = limits<float>(hsv.s, 0.0f, 1.0f);
	hsv.v = limits<float>(hsv.v, 0.0f, 1.0f);

	CHSV chsv;

	if (hsv.h < 0) hsv.h += static_cast<int>(hsv.h) + 1;

	if (hsv.h > 1) hsv.h -= static_cast<int>(hsv.h);

	chsv.hue = static_cast<uint8_t>(hsv.h * 255);
	chsv.sat = static_cast<uint8_t>(hsv.s * 255);
	chsv.val = static_cast<uint8_t>(hsv.v * brightness * 255);
	CRGB crgb;
	hsv2rgb_rainbow(chsv, crgb);
	RGB rgb = { crgb.r, crgb.g, crgb.b };
	return rgb;
}

struct LED
{
	LED(int i) { index = i; };

	int index;

	HSV hsv;
};