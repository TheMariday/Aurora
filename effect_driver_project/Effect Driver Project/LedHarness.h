#pragma once
#include <vector>
#include <string>
#include <map>
#include "LED.h"

class Harness
{
public:
	Harness(int max_leds = 3136);
	~Harness();

	void LoadCalibration(std::string filename);

	int RenderToScreen(bool wait = true, float time = 0.0f);

	LED* GetLed(int index);


	std::vector<LED*> GetMap(std::string mapName = "main");

	void Black();

private:
	std::vector<LED> m_leds;
	std::map<std::string, std::vector<LED*>> m_maps;

	int m_frame = 0;
};