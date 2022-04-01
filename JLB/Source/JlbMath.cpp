#include "JlbMath.h"
#include <cassert>

namespace jlb
{
	float Math::Clamp(float clampable, float min, float max)
	{
		assert(min < max);
		return clampable > max ? max : clampable < min ? min : clampable;
	}
}
