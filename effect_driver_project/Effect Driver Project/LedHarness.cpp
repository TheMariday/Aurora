#include "LedHarness.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <sstream>


Harness::Harness(LedBuffer* ledBuffer, std::string calibrationFile) : m_led_buffer(ledBuffer)
{
	std::ifstream file(calibrationFile);
	if (!file.is_open()) return;

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
	}
	
	return m_group.at(groupName);
}

bool Harness::InGroup(std::string groupName, LED* pLED)
{
	if (m_group.find(groupName) == m_group.end())
	{
		std::cout << "GetGroup found zero groups with name " << groupName << std::endl;
	}

	return std::find(m_group.at(groupName).begin(), m_group.at(groupName).end(), pLED) != m_group.at(groupName).end();
}


int Harness::RenderToScreen(bool wait, int beat)
{
	float scale = 0.3f;
	cv::Mat image = cv::Mat::zeros(static_cast<int>(2400 * scale), static_cast<int>(3500 * scale), CV_8UC3);
	image.setTo(cv::Scalar(30, 30, 30));

	for (LED* led : GetGroup("main"))
	{
		float u, v;

		Loc loc = GetLoc(led);
		RGB rgb = HSV2RGB(led->hsv);
		cv::Vec3b col = cv::Vec3b(rgb.b, rgb.g, rgb.r);

		u = (-loc.z * scale) + image.rows / 2;

		v = (loc.x + 1000) * scale;

		if (loc.y > 0)
			v = (loc.x + 2700) * scale;

		cv::rectangle(image, cv::Rect(static_cast<int>(v) - 1, static_cast<int>(u)-1, 3, 3), col);

		v = (-loc.y + 200) * scale;

		if (loc.x > 0)
			v = (loc.y + 1800) * scale;

		cv::rectangle(image, cv::Rect(static_cast<int>(v) - 1, static_cast<int>(u) - 1, 3, 3), col);
	}

	std::stringstream ss;
	ss << "frame: " << m_frame << " beat: " << beat;
	cv::putText(image, ss.str(), cv::Point(10, 30), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0));
	m_frame += 1;

	imshow("Suit view", image);
	return cv::waitKey(wait ? 0 : 30);
}

void Harness::CopyGroups(Harness* otherHarness)
{

	for (auto const& group : otherHarness->GetGroups())
	{
		std::string groupName = group.first;
		std::vector<LED*> leds = group.second;

		if (m_group.find(groupName) == m_group.end()) // if group is not in local groups
		{
			//add it
			m_group[groupName] = leds;
		}
	}

}
