#pragma once
#include "Effect.h"
#include "Drivable.h"

enum class MaskMix
{
	ADD,
	MULTIPLY,
	CONSTANT
};

class Mask : public Drivable
{
public:
	Mask(Harness* harness) : m_harness(harness)
	{
	}

	float GetModifiedAlpha(LED* led)
	{
		float alpha = GetAlpha(led);
		if (invert) alpha = 1.0f - alpha;
		alpha *= m_intensity;

		return alpha;
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
	MaskMix m_maskMix = MaskMix::MULTIPLY;

private:
	Harness* m_harness;
	bool invert = false;
};
