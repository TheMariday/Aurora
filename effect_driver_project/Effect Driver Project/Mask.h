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
	Mask(Harness* harness, int invert = false, int threshold = -1, float intensity = 1.0f) : 
		m_harness(harness), m_invert(invert), m_threshold(threshold), m_intensity(intensity)
	{
	}

	float GetModifiedAlpha(LED* led)
	{
		float alpha = GetAlpha(led);
		if (m_invert) alpha = 1.0f - alpha;

		if (m_threshold > 0) alpha = alpha > m_threshold ? 1.0f : 0.0f;

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

	float m_intensity;
	bool m_invert;
	int m_threshold;;
	MaskMix m_maskMix = MaskMix::MULTIPLY;

private:
	Harness* m_harness;

};
