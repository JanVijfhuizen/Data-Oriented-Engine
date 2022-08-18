﻿#include "JlbMath.h"

#include <glm/geometric.hpp>

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

	glm::ivec2 RoundNearest(const glm::vec2& v)
	{
		return {roundf(v.x), roundf(v.y)};
	}

	glm::vec2 Clamp(glm::vec2 clampable, const glm::vec2 min, const glm::vec2 max)
	{
		clampable.x = Clamp(clampable.x, min.x, max.x);
		clampable.y = Clamp(clampable.y, min.y, max.y);
		return clampable;
	}

	glm::vec2 Threshold(glm::vec2 clampable, const glm::vec2 min, const glm::vec2 max)
	{
		clampable.x = Threshold(clampable.x, min.x, max.x);
		clampable.y = Threshold(clampable.y, min.y, max.y);
		return clampable;
	}

	glm::vec2 GetDir(const float angle)
	{
		return { cosf(angle), sinf(angle)};
	}

	glm::vec2 Rotate(const glm::vec2 v, const float angle)
	{
		float newX = v.x * cosf(angle) - v.y * sinf(angle);
		float newY = v.x * sinf(angle) + v.y * cosf(angle);
		return { newX, newY };
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

	glm::vec2 LerpClamped(const glm::vec2& a, const glm::vec2& b, const float delta)
	{
		const glm::vec2 offset = b - a;
		const glm::vec2 ret = a + offset * Min<float>(delta, glm::distance(a, b));
		return ret;
	}

	bool IsZero(const glm::ivec2& v)
	{
		return v.x == 0 && v.y == 0;
	}
}
