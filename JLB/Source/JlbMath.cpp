#include "JlbMath.h"
#include <cassert>

namespace jlb::math
{
	float GetAngle(const glm::vec2& a, const glm::vec2& b)
	{
		return atan2(a.y - b.y, a.x - b.x) + PI / 2;
	}

	float SmoothAngle(float a, float b, const float delta)
	{
		const float diff = abs(a - b);
		const bool diffMoreThanPI = diff > PI;

		const float nA = a + (diffMoreThanPI ? (b > a ? PI * 2 : 0) : 0);
		const float nB = b + (diffMoreThanPI ? (a > b ? PI * 2 : 0) : 0);

		return WrapAngle(Lerp(nA, nB, delta));
	}

	float WrapAngle(const float f)
	{
		return fmod(f, PI * 2);
	}

	float Lerp(const float a, const float b, const float delta)
	{
		return a + delta * ((b > a) * 2 - 1);
	}

	float LerpPct(const float a, const float b, const float pct)
	{
		return a + (b - a) * pct;
	}

	bool IsZero(const glm::ivec2& v)
	{
		return v.x == 0 && v.y == 0;
	}
}
