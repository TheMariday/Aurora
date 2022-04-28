#pragma once
#include "Drivable.h"
#include "LedHarness.h"

class Texture : public Drivable
{
public:
	Texture(Harness* harness) : m_harness(harness) {}

	virtual HSV TextureLed(LED* led) { return { 1.0f, 1.0f, 1.0f }; }

	Harness* GetHarness() { return m_harness; }

private:
	Harness* m_harness;
};