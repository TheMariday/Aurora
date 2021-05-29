#pragma once
#include <string>

namespace TEF::Aurora
{
	struct LED
	{
		void Black()
		{
			r = 0; g = 0; b = 0;
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