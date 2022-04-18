#pragma once

#include <chrono>
#include <thread>
#include <functional>
#include <vector>
#include "LedHarness.h"
#include "utils.h"


enum class EffectState
{
	READY,
	RUNNING,
	STOPPED
};

class Effect
{
public:
	Effect(Harness* harness, timestamp start = Now(), timestamp end = MaxTimestamp());

	Effect(Harness* harness, timestamp start, duration dur);

	~Effect();

	void Update(timestamp t);

	virtual void Render(timestamp t) {};

	void AddDriver(std::function<void(timestamp t)> a);

	void AddSubEffect(Effect e);

	void Stop();

	timestamp GetStartTime();

	bool HasStopped();

	Harness* GetHarness() { return m_harness; }

	std::vector<LED*> GetLeds();

	void SetMask(mask m) { m_mask = m; }

private:

	timestamp m_startTime;
	timestamp m_endTime;
	Harness* m_harness;

	std::vector<Driver> m_drivers;

	mask m_mask;

	EffectState m_state = EffectState::READY;

	std::vector<Effect> m_subEffects;
};