#include "LedHarness.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

class HarnessCV : public Harness
{
public:

	HarnessCV(LedBuffer* ledBuffer, std::string calibrationFile) 
		:Harness(ledBuffer, calibrationFile) 
	{
	};

	bool Render() override
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
				v = (-loc.x + 2700) * scale;

			cv::rectangle(image, cv::Rect(static_cast<int>(v) - 1, static_cast<int>(u) - 1, 3, 3), col);

			v = (-loc.y + 200) * scale;

			if (loc.x > 0)
				v = (loc.y + 1800) * scale;

			cv::rectangle(image, cv::Rect(static_cast<int>(v) - 1, static_cast<int>(u) - 1, 3, 3), col);
		}

		imshow("Suit view", image);
		return cv::waitKey(m_wait ? 0 : 30) != 27;
	}
	bool m_wait = true;
};