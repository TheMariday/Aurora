#pragma once

#include <chrono>
#include <thread>
#include <functional>
#include <vector>
#include "LedHarness.h"

#define timestamp std::chrono::system_clock::time_point
#define duration std::chrono::milliseconds
#define Duration_cast std::chrono::duration_cast<duration>
#define Driver std::function<void(timestamp t)>
#define MaxTimestamp std::chrono::time_point<std::chrono::system_clock>::max
#define Now std::chrono::system_clock::now

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

	virtual void Render(Harness* harness, timestamp t) {};

	void AddDriver(std::function<void(timestamp t)> a);

	void AddSubEffect(Effect e);

	void Stop();

	timestamp GetStartTime();

private:

	timestamp m_startTime;
	timestamp m_endTime;
	Harness* m_harness;

	std::vector<Driver> m_drivers;

	EffectState m_state = EffectState::READY;

	std::vector<Effect> m_subEffects;
};