#include "LedHarness.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <sstream>


Harness::Harness(int max_leds)
{
	m_leds.resize(max_leds);
}

Harness::~Harness()
{
}

void Harness::LoadCalibration(std::string filename)
{
	std::ifstream file(filename);
	if (!file.is_open()) return;

	std::string line, word;


	while (std::getline(file, line))
	{
		std::stringstream str(line);
		std::vector<std::string> row;
		while (getline(str, word, ','))
			row.push_back(word);

		int index = std::stoi(row[0]);
		m_leds.at(index).loc.x = std::stoi(row[1]); // this is essentially capping the resolution to 1mm but for the sake of int math speed I'm going to do it
		m_leds.at(index).loc.y = std::stoi(row[2]);
		m_leds.at(index).loc.z = std::stoi(row[3]);

		for (int i = 4; i < row.size(); ++i)
			m_maps[row[i]].push_back(&m_leds.at(index));

	}

	file.close();

}

int Harness::RenderToScreen(bool wait, float time)
{
	float scale = 0.3;
	cv::Mat image = cv::Mat::zeros(2400 * scale, 3500 * scale, CV_8UC3);
	image.setTo(cv::Scalar(30, 30, 30));

	for (const LED& led : m_leds)
	{
		int u, v;

		u = (-led.loc.z * scale) + image.rows / 2;


		v = (led.loc.x + 1000) * scale;

		if (led.loc.y > 0)
			v = (led.loc.x + 2700) * scale;
		cv::Vec3b col = cv::Vec3b(led.rgb.b, led.rgb.g, led.rgb.r);

		cv::rectangle(image, cv::Rect(v, u, 2, 2), col);

		v = (-led.loc.y + 200) * scale;

		if (led.loc.x > 0)
			v = (led.loc.y + 1800) * scale;

		cv::rectangle(image, cv::Rect(v, u, 2, 2), col);
	}

	std::stringstream ss;
	ss << "frame: " << m_frame << " time: " << time;
	cv::putText(image, ss.str(), cv::Point(10, 30), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0));
	m_frame += 1;

	imshow("Suit view", image);
	return cv::waitKey(wait ? 0 : 30);


}

LED* Harness::GetLed(int index)
{
	return nullptr;
}

std::vector<LED*> Harness::GetMap(std::string mapName)
{
	return m_maps[mapName];
}
