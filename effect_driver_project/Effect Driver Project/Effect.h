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

enum class EffectState
{
	READY,
	RUNNING,
	STOPPED
};

class Effect
{
public:
	Effect(timestamp t);
	~Effect();

	void Update(Harness& harness, timestamp t);

	virtual void Render(Harness& harness, timestamp t) = 0;

	void AddDriver(std::function<void(timestamp t)> a);

	void Stop();

	timestamp GetStartTime();

private:

	std::vector<Driver> m_drivers;

	timestamp m_startTime;

	EffectState m_state = EffectState::READY;
};