#pragma once
#include "LED.h"
#include "LedHarness.h"
#include <vector>
#include "utils.h"
#include "LED.h"
#include <cmath>

void Rainbow(Harness* h, std::vector<LED*> leds, Loc center, float offset = 0.0f, axis ax = x_axis, bool flip = false)
{
	for (LED* led : leds)
	{
		Loc loc = h->GetLoc(led);

		double radAngle = 0;

		switch (ax)
		{
		case x_axis:
			radAngle = atan2(loc.y - center.y, loc.z - center.z);
			break;
		case y_axis:
			radAngle = atan2(loc.z - center.z, loc.x - center.x);
			break;
		case z_axis:
			radAngle = atan2(loc.y - center.y, loc.x - center.x);
			break;
		}

		double normAngle = fmod(static_cast<float>(radAngle / (M_PI * 2)) + offset, 1.0f);
		if (flip)
			normAngle = 1 - normAngle;

		led->hsv = { static_cast<float>(normAngle), 1.0f, 1.0f };
	}
}