#pragma once
#include <vector>
#include <string>
#include <map>
#include "LED.h"

class LedBuffer
{
public:
	LedBuffer(int size = 3136) {
		for (int i = 0; i < size; ++i)
			leds.push_back(i);
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

	std::vector<LED> leds;
};

class Harness
{
public:
	Harness(LedBuffer* ledBuffer, std::string calibrationFile);
	~Harness();

	Loc GetLoc(LED* led);

	Loc GetMarker(std::string groupName);

	std::vector<LED*> GetGroup(std::string groupName);

	bool InGroup(std::string groupName, LED* pLED);

	int RenderToScreen(bool wait = true, int beat = 0);

	std::map<std::string, std::vector<LED*>> GetGroups() { return m_group; };

	void CopyGroups(Harness* otherHarness);

	Loc GetRandomLoc(bool backOnly = false);

private:
	std::map<std::string, std::vector<LED*>> m_group;
	std::map<LED*, Loc> m_loc;

	int m_frame = 0;

	LedBuffer* m_led_buffer;
};