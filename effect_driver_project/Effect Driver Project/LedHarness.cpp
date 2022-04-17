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


int Harness::RenderToScreen(bool wait, float time)
{
	float scale = 0.3;
	cv::Mat image = cv::Mat::zeros(2400 * scale, 3500 * scale, CV_8UC3);
	image.setTo(cv::Scalar(30, 30, 30));

	for (LED* led : GetGroup("main"))
	{
		int u, v;

		Loc loc = GetLoc(led);
		cv::Vec3b col = cv::Vec3b(led->rgb.b, led->rgb.g, led->rgb.r);

		u = (-loc.z * scale) + image.rows / 2;

		v = (loc.x + 1000) * scale;

		if (loc.y > 0)
			v = (loc.x + 2700) * scale;

		cv::rectangle(image, cv::Rect(v, u, 2, 2), col);

		v = (-loc.y + 200) * scale;

		if (loc.x > 0)
			v = (loc.y + 1800) * scale;

		cv::rectangle(image, cv::Rect(v, u, 2, 2), col);
	}

	std::stringstream ss;
	ss << "frame: " << m_frame << " time: " << time;
	cv::putText(image, ss.str(), cv::Point(10, 30), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0));
	m_frame += 1;

	imshow("Suit view", image);
	return cv::waitKey(wait ? 0 : 30);


}
