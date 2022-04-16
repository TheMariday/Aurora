#pragma once
#include "Effect.h"
#include "LED.h"


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