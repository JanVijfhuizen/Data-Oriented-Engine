#include "JlbMath.h"

namespace jlb::math
{
	float GetAngle(const glm::vec2& a, const glm::vec2& b)
	{
		return atan2f(a.y - b.y, a.x - b.x) + PI / 2;
	}

	float SmoothAngle(float a, float b, const float delta)
	{
		const float diff = abs(b - a);
		const bool diffMoreThanPi = diff > PI;
		const bool bMoreThanA = b > a;

		a += diffMoreThanPi && bMoreThanA ? PI * 2 : 0;
		b += diffMoreThanPi && !bMoreThanA ? PI * 2 : 0;

		const float value = a + (b - a) * delta;
		return WrapAngle(value);
	}

	float WrapAngle(const float f)
	{
		return fmodf(f, PI * 2);
	}

	float Lerp(const float a, const float b, const float delta)
	{
		return a + delta * ((b > a) * 2 - 1);
	}

	float LerpPct(const float a, const float b, const float pct)
	{
		return a + (b - a) * pct;
	}

	glm::vec2 LerpPct(const glm::vec2& a, const glm::vec2& b, const float pct)
	{
		return
		{
			LerpPct(a.x, b.x, pct),
			LerpPct(a.y, b.y, pct),
		};
	}

	bool IsZero(const glm::ivec2& v)
	{
		return v.x == 0 && v.y == 0;
	}
}
