#include "JlbMath.h"
#include <cassert>

namespace jlb::math
{
	float Clamp(float clampable, float min, float max)
	{
		assert(min < max);
		return clampable > max ? max : clampable < min ? min : clampable;
	}
}
