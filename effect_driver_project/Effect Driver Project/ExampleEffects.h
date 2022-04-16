#pragma once
#include "Effect.h"
#include "hsv2rgb.h"

RGB HSV2RGB_Rainbow(HSV hsv)
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


class SolidColour : public Effect
{
public:
	SolidColour(timestamp t) : Effect(t) {};

	void Render(Harness& harness, timestamp t) override
	{
		RGB rgb = HSV2RGB_Rainbow(hsv);
		for (LED* pLED : harness.GetMap("eyes"))
		{
			pLED->rgb = rgb;
		}
	}

	HSV hsv = { 1.0f, 1.0f, 1.0f };
};