#include "tef/aurora/effectRunner.h"
#include <spdlog/spdlog.h>
#include "tef/aurora/settings.h"

TEF::Aurora::EffectRunner::EffectRunner()
{
	SetFPS(Settings::FPS_EFFECT);
}

TEF::Aurora::EffectRunner::~EffectRunner()
{
	Black();
}

bool TEF::Aurora::EffectRunner::Connect()
{
	m_opc.resolve(Settings::FADECANDY.c_str());

	if (!m_opc.tryConnect())
	{
		spdlog::error("Effect Runner cannot connect to OPC client");
		return false;
	}

	for (int i = 0; i < m_ledCount; i++)
	{
		m_leds.emplace_back();
		m_leds.back().index = i;
	}


	// Set up an empty framebuffer, with OPC packet header
	int frameBytes = m_ledCount * 3;

	m_frameBuffer.resize(sizeof(OPCClient::Header) + frameBytes);
	OPCClient::Header::view(m_frameBuffer).init(0, 0, frameBytes);

	return true;
}

void TEF::Aurora::EffectRunner::Black()
{
	for (LED& led : m_leds)
		led.Black();

	WriteToFC();
}

bool TEF::Aurora::EffectRunner::Disable()
{
	m_enabled = false;
	return true;
}

bool TEF::Aurora::EffectRunner::StopAll()
{
	for (auto& effect : m_effects)
		effect->Stop();
	return true;
}

bool TEF::Aurora::EffectRunner::AddEffect(std::shared_ptr<Effect> effect)
{
	m_effects.push_back(effect);
	return true;
}

bool TEF::Aurora::EffectRunner::MainLoopCallback()
{
	for (LED& led : m_leds)
		led.Black();

	if (m_enabled)
	{
		for (auto& effect : m_effects)
			effect->Render(m_leds);
	}

	return WriteToFC();
}

bool TEF::Aurora::EffectRunner::WriteToFC()
{
	if (!m_opc.tryConnect()) {
		spdlog::error("Effect Runner could not connect to fadecandy board");
		return false;
	}

	uint8_t* dest = OPCClient::Header::view(m_frameBuffer).data();

	for (auto& led : m_leds) {

		for (float c : led.RGB())
		{
			c *= 255;
			c = std::max<float>(c, 0);
			c = std::min<float>(c, 255);
			*(dest++) = c;
		}
	}

	m_opc.write(m_frameBuffer);
	return true;
}
