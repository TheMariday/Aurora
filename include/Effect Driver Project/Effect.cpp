#include "Effect.h"
#pragma once


Effect::Effect(Harness* harness, timestamp start, timestamp end) : m_startTime(start), m_endTime(end), m_harness(harness)
{
}

Effect::Effect(Harness* harness, timestamp start, duration dur) : m_startTime(start), m_endTime(start + dur), m_harness(harness)
{
}

Effect::~Effect()
{
}

void Effect::Update(timestamp t)
{
	if (m_state == EffectState::STOPPED)
		return;

	if (t >= m_startTime)
		m_state = EffectState::RUNNING;

	if (t > m_endTime)
		m_state = EffectState::STOPPED;

	if (m_state != EffectState::RUNNING)
		return;


	for (Effect& effect : m_subEffects)
		effect.Update(t);

	for (const Driver& driver : m_drivers)
		driver(t);

	Render(t);
}

void Effect::AddDriver(std::function<void(timestamp t)> a)
{
	m_drivers.push_back(a);
}

void Effect::AddSubEffect(Effect e)
{
	m_subEffects.push_back(e);
}

void Effect::Stop()
{
	m_state = EffectState::STOPPED;
}

bool Effect::HasStopped()
{
	return m_state == EffectState::STOPPED;
}

std::vector<LED*> Effect::GetLeds()
{
	std::vector<LED*> leds;
	if (m_mask)
		leds = m_mask();
	else
		leds = m_harness->GetGroup("main");
	return leds;
}

timestamp Effect::GetStartTime()
{
	return m_startTime;
}
