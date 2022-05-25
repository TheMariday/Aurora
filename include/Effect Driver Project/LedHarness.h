#pragma once
#include <vector>
#include <string>
#include <map>
#include "LED.h"
#include <fadecandy/opc_client.h>

class LedBuffer
{
public:
	LedBuffer(int size = 3136) {
		for (int i = 0; i < size; ++i)
			leds.push_back(i);

		uint16_t frameBytes = (uint16_t)(size * 3);

		m_frameBuffer.resize(sizeof(OPCClient::Header) + frameBytes);
		OPCClient::Header::view(m_frameBuffer).init(0, 0, frameBytes);
	}

	LED* GetLed(int index)
	{
		return &leds.at(index);
	}

	void Black()
	{
		for (LED& led : leds)
			led.hsv = {};
	}

	void UpdateFrameBuffer()
	{
		uint8_t* dest = OPCClient::Header::view(m_frameBuffer).data();

		for (auto& led : leds) {
			RGB rgb = HSV2RGB(led.hsv, m_brightness);
			for (int i = 0; i < 3; ++i)
			{
				int v = rgb[i];
				v = std::max<int>(v, 0);
				v = std::min<int>(v, 255);
				*(dest++) = (uint8_t)v;
			}
		}
	}

	std::vector<LED> leds;
	std::vector<uint8_t> m_frameBuffer;

	float m_brightness = 1.0f;
};

class Harness
{
public:
	Harness(LedBuffer* ledBuffer = nullptr, std::string calibrationFile = "");
	~Harness();

	Loc GetLoc(LED* led);

	Loc GetMarker(std::string groupName);

	std::vector<LED*> GetGroup(std::string groupName);

	bool InGroup(std::string groupName, LED* pLED);

	virtual bool Render();

	std::map<std::string, std::vector<LED*>> GetGroups() { return m_group; };

	void CopyGroups(Harness* otherHarness);

	Loc GetRandomLoc(bool backOnly = false);

private:
	std::map<std::string, std::vector<LED*>> m_group;
	std::map<LED*, Loc> m_loc;

	int m_frame = 0;

	LedBuffer* m_led_buffer;
};