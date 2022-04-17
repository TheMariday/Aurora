#pragma once
#include "LED.h"
#include "LedHarness.h"
#include <vector>
#include "utils.h"
#include "LED.h"

void Rainbow(Harness* h, std::vector<LED*> leds, Loc center, axis ax = x_axis, bool flip=false)
{
	for (LED* led : leds)
	{
		Loc loc = h->GetLoc(led);

		double radAngle;

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

		double normAngle = radAngle / (M_PI * 2);
		if (flip)
			normAngle = 1 - normAngle;

		led->hsv = { static_cast<float>(normAngle), 1.0f, 1.0f };
	}
}