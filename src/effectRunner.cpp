#include "tef/aurora/effectRunner.h"
#include <spdlog/spdlog.h>

TEF::Aurora::EffectRunner::EffectRunner(std::string hostport)
{
	SetFPS(60);
	m_opc.resolve(hostport.c_str());

	if (m_opc.tryConnect())
	{
		spdlog::error("cannot connect");
	}
}

TEF::Aurora::EffectRunner::~EffectRunner()
{
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

	for (auto& effect : m_effects)
		effect->Render(m_leds);

	// send m_leds buffer to hardware
	return true;
}

bool TEF::Aurora::EffectRunner::WriteToFC()
{
	if (!m_opc.tryConnect()) {
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
