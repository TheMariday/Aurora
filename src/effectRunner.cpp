#include "tef/aurora/effectRunner.h"
#include <spdlog/spdlog.h>

TEF::Aurora::EffectRunner::EffectRunner()
{

}

TEF::Aurora::EffectRunner::~EffectRunner()
{
	spdlog::info("Effect Runner being destroyed, stopping mainloop");
	Stop();
	spdlog::info("Setting all leds to black");
	Black();
}

bool TEF::Aurora::EffectRunner::Connect(std::string address)
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

	for (int i = 0; i < m_ledCount; i++)
	{
		m_leds.emplace_back();
		m_leds.back().index = i;
	}


	// Set up an empty framebuffer, with OPC packet header
	uint16_t frameBytes = (uint16_t)(m_ledCount * 3);

	m_frameBuffer.resize(sizeof(OPCClient::Header) + frameBytes);
	OPCClient::Header::view(m_frameBuffer).init(0, 0, frameBytes);

	m_connected = true;

	return true;
}

void TEF::Aurora::EffectRunner::Black()
{
	for (LED& led : m_leds)
		led.Black();

	WriteToFC();
	WriteToFC();
}

bool TEF::Aurora::EffectRunner::Disable()
{
	m_enabled = false;
	StopAll();
	Black();
	return true;
}

bool TEF::Aurora::EffectRunner::StopAll()
{
	for (auto const& [order, uidToEffect] : m_effects)
		for (auto const& [uid, effect] : uidToEffect)
			effect->Stop();

	return true;
}

bool TEF::Aurora::EffectRunner::AddEffect(std::string uid, std::shared_ptr<Effect> effect, int order)
{
	if (m_effects[order].find(uid) != m_effects[order].end()) {
		spdlog::warn("Effect Runner has tried to register an effect over an existing one");
	}

	m_effects[order][uid] = effect;

	return true;
}

bool TEF::Aurora::EffectRunner::GetEffect(std::string uid, std::shared_ptr<Effect>& effect)
{
	for (auto const& [order, uidToEffect] : m_effects)
	{
		if (uidToEffect.find(uid) != uidToEffect.end())
		{
			effect = uidToEffect.at(uid);
			return true;
		}
	}

	return false;
}

bool TEF::Aurora::EffectRunner::RemoveEffect(std::string uid)
{
	int effectsRemoved = 0;
	for (auto& [order, uidToEffect] : m_effects)
	{
		effectsRemoved += uidToEffect.erase(uid);
	}

	return effectsRemoved;
}


bool TEF::Aurora::EffectRunner::MainLoopCallback()
{
	for (LED& led : m_leds)
		led.Black();

	// This takes advantage of the fact that maps are ordered by key by default
	if (m_enabled)
	{
		for (auto const& [order, uidToEffect] : m_effects)
		{
			for (auto const& [uid, effect] : uidToEffect)
			{
				if (effect->IsRunning())
					effect->Render(m_leds);
			}
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

	uint8_t* dest = OPCClient::Header::view(m_frameBuffer).data();

	for (auto& led : m_leds) {

		for (float c : led.RGB())
		{
			c *= 255;
			c = std::max<float>(c, 0);
			c = std::min<float>(c, 255);
			*(dest++) = (uint8_t)c;
		}
	}

	m_opc.write(m_frameBuffer);
	return true;
}
