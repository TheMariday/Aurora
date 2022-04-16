#pragma once
#include <vector>
#include <string>
#include <map>

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


struct LED
{
	RGB rgb;
	Loc loc;

	bool mapped = false;
};

static inline RGB HSV2RGB(HSV hsv)
{
	hsv.h = fmodf(hsv.h, 1) * 6.0f;
	if (hsv.h < 0) hsv.h += 6.0f;

	float r = 0, g = 0, b = 0;

	int i = hsv.h;
	float f = hsv.h - i;
	float p = hsv.v * (1 - hsv.s);
	float q = hsv.v * (1 - f * hsv.s);
	float t = hsv.v * (1 - (1 - f) * hsv.s);

	switch (i) {
	case 0: r = hsv.v;	g = t;		b = p;		break;
	case 1: r = q;		g = hsv.v;	b = p;		break;
	case 2: r = p;		g = hsv.v;	b = t;		break;
	case 3: r = p;		g = q;		b = hsv.v;	break;
	case 4: r = t;		g = p;		b = hsv.v;	break;
	case 5: r = hsv.v;	g = p;		b = q;		break;
	}
	RGB rgb;
	rgb.r = r * 255;
	rgb.g = g * 255;
	rgb.b = b * 255;

	return rgb;
}

class Harness
{
public:
	Harness(int max_leds = 3136);
	~Harness();

	void LoadCalibration(std::string filename);

	int RenderToScreen(bool wait = true, float time = 0.0f);

	LED* GetLed(int index);

	std::vector<LED*> GetMap(std::string mapName = "main");

private:
	std::vector<LED> m_leds;
	std::map<std::string, std::vector<LED*>> m_maps;

	int m_frame = 0;
};