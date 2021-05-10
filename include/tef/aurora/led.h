#pragma once
#include "svl/SVL.h"


namespace TEF::Aurora
{
	struct LED {
		Vec3 position;

		bool isValid() {
			return position != Vec3(0, 0, 0);
		}
	};
}