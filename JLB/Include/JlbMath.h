#pragma once
#include <glm/vec2.hpp>

namespace jlb::math
{
	constexpr float PI = 3.14159265358979323846;

	// Get the angle between two points.
	[[nodiscard]] float GetAngle(const glm::vec2& a, const glm::vec2& b);

	// Smooth an angle from a to b by the given value.
	[[nodiscard]] float SmoothAngle(float a, float b, float delta);

	// Wraps an radians angle.
	[[nodiscard]] float WrapAngle(float f);

	// Linearly interpolates between two values by a flat value.
	[[nodiscard]] float Lerp(float a, float b, float delta);
	// Linearly interpolates between to values from 0 to 1.
	[[nodiscard]] float LerpPct(float a, float b, float pct);

	// Linearly interpolates between to vectors from 0 to 1.
	[[nodiscard]] glm::vec2 LerpPct(const glm::vec2& a, const glm::vec2& b, float pct);

	// Checks if the length of the vector is zero.
	[[nodiscard]] bool IsZero(const glm::ivec2& v);

	// Clamp a value between a minimum and a maximum number.
	template <typename T>
	[[nodiscard]] T Clamp(T clampable, T min, T max);

	// Returns the highest of the two values.
	template <typename T>
	[[nodiscard]] T Max(T a, T b);

	// Returns the lowest of the two values.
	template <typename T>
	[[nodiscard]] T Min(T a, T b);

	template <typename T>
	[[nodiscard]] T Clamp(const T clampable, const T min, const T max)
	{
		assert(min < max);
		return clampable > max ? max : clampable < min ? min : clampable;
	}

	template <typename T>
	T Max(const T a, const T b)
	{
		return a < b ? b : a;
	}

	template <typename T>
	T Min(T a, T b)
	{
		return a > b ? b : a;
	}
}
