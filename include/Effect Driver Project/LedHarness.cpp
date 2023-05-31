#include "LedHarness.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>


Harness::Harness(LedBuffer* ledBuffer, std::string calibrationFile) : m_led_buffer(ledBuffer)
{
	if (calibrationFile.empty()) return;
	if (!ledBuffer) return;

	std::ifstream file(calibrationFile);
	if (!file.is_open())
	{
		std::cout << "failed to load calibration file" << std::endl;
		return;
	}
	

	std::string line, word;


	while (std::getline(file, line))
	{
		std::stringstream str(line);
		std::vector<std::string> row;
		while (getline(str, word, ','))
			row.push_back(word);

		int index = std::stoi(row[0]);
		Loc loc = { std::stoi(row[1]) , std::stoi(row[2]) , std::stoi(row[3]) };

		LED* pLED = m_led_buffer->GetLed(index);
		m_loc[pLED] = loc;

		for (int i = 4; i < row.size(); ++i)
		{
			std::string groupName = row[i];
			m_group[groupName].push_back(pLED);
		}
	}

	file.close();
}

Harness::~Harness()
{
}

Loc Harness::GetLoc(LED* led)
{
	if (m_loc.find(led) == m_loc.end())
	{
		std::cout << "cannot find led location" << std::endl;
	}

	return m_loc.at(led);
}

Loc Harness::GetMarker(std::string groupName)
{

	if (groupName == "center")
		return { 0,0,0 };

	std::vector<LED*> leds = GetGroup(groupName);
	if (leds.size() == 0)
	{
		std::cout << "GetMarker found zero markers with name " << groupName << std::endl;
	}
	return GetLoc(leds.at(0));
}

std::vector<LED*> Harness::GetGroup(std::string groupName)
{
	if (m_group.find(groupName) == m_group.end())
	{
		std::cout << "GetGroup found zero groups with name " << groupName << std::endl;
		return std::vector<LED*>();
	}

	return m_group.at(groupName);
}

bool Harness::InGroup(std::string groupName, LED* pLED)
{
	if (m_group.find(groupName) == m_group.end())
	{
		std::cout << "GetGroup found zero groups with name " << groupName << std::endl;
		return false;
	}

	return std::find(m_group.at(groupName).begin(), m_group.at(groupName).end(), pLED) != m_group.at(groupName).end();
}


bool Harness::Render()
{
	std::cout << "not implemented" << std::endl;
	return false;
}

void Harness::CopyGroups(Harness* otherHarness)
{
	for (auto const& group : otherHarness->GetGroups())
	{
		std::string groupName = group.first;
		std::vector<LED*> leds = group.second;

		m_group[groupName] = leds;
		
	}
}

Loc Harness::GetRandomLoc(bool backOnly)
{
	std::vector<LED*> leds = GetGroup("head");
	int random = rand() % leds.size();
	Loc loc = GetLoc(leds[random]);

	if (backOnly && loc.y < 0)
		return GetRandomLoc(backOnly);
	else
		return loc;
}
