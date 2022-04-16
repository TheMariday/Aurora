#include "Effect.h"
#pragma once


Effect::Effect(timestamp t)
{
	m_startTime = t;
}

Effect::~Effect()
{
}

void Effect::Update(Harness& harness, timestamp t)
{
	if (m_state == EffectState::STOPPED)
		return;

	if (t >= m_startTime)
		m_state = EffectState::RUNNING;

	if (m_state != EffectState::RUNNING)
		return;

	for (Effect& effect : m_subEffects)
		effect.Update(harness, t);

	for (const Driver& driver : m_drivers)
		driver(t);

	Render(harness, t);
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

timestamp Effect::GetStartTime()
{
	return m_startTime;
}
