#pragma once

namespace jlb::math
{
	// Clamp a value between a minimum and a maximum number.
	[[nodiscard]] float Clamp(float clampable, float min, float max);

	// Returns the highest of the two values.
	template <typename T>
	[[nodiscard]] T Max(T a, T b);

	// Returns the lowest of the two values.
	template <typename T>
	[[nodiscard]] T Min(T a, T b);

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
