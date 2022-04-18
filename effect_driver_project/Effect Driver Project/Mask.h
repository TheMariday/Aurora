#pragma once
#include "Effect.h"




std::vector<LED*> OrbMask(Harness* harness, std::vector<LED*> leds, Loc center, int diameter, bool inner = true)
{
	std::vector<LED*> masked;

	if (diameter < 0) return masked;

	for (LED* led : leds)
	{
		Loc loc = harness->GetLoc(led);

		int distance = Distance(loc, center);
		if (inner)
		{
			if (distance < diameter / 2)
				masked.emplace_back(led);
		}
		else
		{
			if (distance > diameter / 2)
				masked.emplace_back(led);
		}
	}
	return masked;
}

std::vector<LED*> RingMask(Harness* harness, std::vector<LED*> leds, Loc center, int diameter, int ringWidth)
{
	std::vector<LED*> ringMask = OrbMask(harness, leds, center, diameter);
	return OrbMask(harness, ringMask, center, diameter - ringWidth, false);
}