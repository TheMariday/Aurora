#include "tef/aurora/effectRunner.h"
#include <spdlog/spdlog.h>
#include <stdio.h>
#include <iostream>

TEF::Aurora::EffectRunner::EffectRunner()
{
	SetFPS(60);
}

TEF::Aurora::EffectRunner::~EffectRunner()
{
	spdlog::info("Effect Runner being destroyed, stopping mainloop");
	StopAll();
	Stop();
	spdlog::info("Setting all leds to black");
	m_ledBuffer.Black();
	WriteToFC();
}

bool TEF::Aurora::EffectRunner::Connect(std::string address, SmartFuse* smartFuse)
{

	if (!m_opc.resolve(address.c_str()))
	{
		spdlog::error("Effect Runner failed to resolve OPC address");
		return false;
	}

	if (!m_opc.tryConnect())
	{
		spdlog::error("Effect Runner cannot connect to OPC client");
		return false;
	}

	m_smartFuse = smartFuse;

	m_harness = Harness(&m_ledBuffer, "/home/pi/led_positions.csv");

	m_connected = true;

	return true;
}

bool TEF::Aurora::EffectRunner::Disable()
{
	m_enabled = false;
	StopAll();
	m_ledBuffer.Black();
	WriteToFC();
	return true;
}

bool TEF::Aurora::EffectRunner::StopAll()
{
	for (auto e : m_effects)
		e->Stop();

	return true;
}

bool TEF::Aurora::EffectRunner::AddEffect(std::shared_ptr<Effect> effect)
{

	if (m_effects.size() == 1) // if we've just added an effect onto the eyes
	{
		m_smartFuse->StartAll();
		spdlog::info("opening fuse system");
	}

	m_effects.push_back(effect);

	return true;
}

bool TEF::Aurora::EffectRunner::MainLoopCallback()
{
	timestamp t = Now();

	for (LED& led : m_ledBuffer.leds)
	{
		led.hsv.v *= 0.9f;
		led.hsv.s *= 0.9f;
	}

	for (auto e : m_effects)
		e->Update(t);

	//remove dead sub effects
	int effectSizePrior = m_effects.size();
	m_effects.erase(std::remove_if(m_effects.begin(), m_effects.end(), [](const std::shared_ptr<Effect>& x) {return x->HasStopped(); }), m_effects.end());

	if (m_smartFuse && m_smartFuse->IsConnected())
	{
		if (m_effects.size() == 1 && effectSizePrior > 1) // if we've just lost an effect and down to just 1 effect, which is the eyes
		{
			m_smartFuse->StopAll();
			spdlog::info("closing fuse system");
		}
	}

	return WriteToFC();
}

bool TEF::Aurora::EffectRunner::WriteToFC()
{
	if (!m_opc.tryConnect()) {
		spdlog::error("Effect Runner could not connect to fadecandy board");
		return false;
	}

	m_ledBuffer.UpdateFrameBuffer();
	m_opc.write(m_ledBuffer.m_frameBuffer);
	return true;
}
