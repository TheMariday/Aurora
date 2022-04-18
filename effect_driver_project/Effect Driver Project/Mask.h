#pragma once
#include "Effect.h"
#include "Drivable.h"


class Mask : public Drivable
{
public:
	Mask(Harness* harness) : m_harness(harness)
	{
	}

	std::vector<std::pair<LED*, float>> GetLEDs()
	{
		std::vector<std::pair<LED*, float>> leds_out;
		for (LED* led : GetHarness()->GetGroup("main"))
		{
			float alpha = GetAlpha(led);
			if (invert) alpha = 1.0f - alpha;
			alpha *= m_intensity;
			if (alpha)
				leds_out.push_back(std::make_pair(led, alpha));
		}
		return leds_out;
	}

	virtual float GetAlpha(LED* led)
	{
		return 1.0f;
	}

	Harness* GetHarness() 
	{
		return m_harness;
	}

	float m_intensity = 1.0f;

private:
	Harness* m_harness;
	bool invert = false;
};
