#pragma once
#include <string>
#include <math.h>

namespace TEF::Aurora
{
	struct LED
	{
		void Black()
		{
			r = 0; g = 0; b = 0;
		}

		void HSV(float h, float s, float v)
		{
			h = fmodf(h, 1) * 6.0f;
			if (h < 0) h += 6.0f;

			int i = static_cast<int>(h);
			float f = h - static_cast<float>(i);
			float p = v * (1 - s);
			float q = v * (1 - f * s);
			float t = v * (1 - (1 - f) * s);

			switch (i) {
			case 0: r = v; g = t; b = p; break;
			case 1: r = q; g = v; b = p; break;
			case 2: r = p; g = v; b = t; break;
			case 3: r = p; g = q; b = v; break;
			case 4: r = t; g = p; b = v; break;
			case 5: r = v; g = p; b = q; break;
			}
		}

		void RGB(float rIn, float gIn, float bIn)
		{
			r = rIn;
			g = gIn;
			b = bIn;
		}

		std::vector<float> RGB()
		{
			return { r, g, b };
		};

		float r, g, b;
		float x, y, z;
		int index;
		std::string name;
	};
}