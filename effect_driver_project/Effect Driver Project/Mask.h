#pragma once
#include "Effect.h"

class Mask
{

	virtual std::vector<LED*> GetMask(std::vector<LED*> leds, bool invert = false) = 0;
};

struct OrbMask : public Mask
{
	std::vector<LED*> GetMask(std::vector<LED*> leds, bool invert = false)
	{
		std::vector<LED*> masked;

		if (diameter < 0) return masked;

		for (LED* led : leds)
		{
			int distance = Distance(led->loc, center);
			if (distance < diameter / 2)
				masked.emplace_back(led);
		}
		return masked;
	}

	Loc center;
	int diameter;
};