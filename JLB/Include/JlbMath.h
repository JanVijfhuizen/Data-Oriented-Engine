#pragma once

namespace jlb
{
	/// <summary>
	/// Basic math class that contains some useful mathematical functions.
	/// </summary>
	class Math final
	{
	public:
		// Clamp a value between a minimum and a maximum number.
		[[nodiscard]] static float Clamp(float clampable, float min, float max);

		// Returns the highest of the two values.
		template <typename T>
		[[nodiscard]] static T Max(T a, T b);

		// Returns the lowest of the two values.
		template <typename T>
		[[nodiscard]] static T Min(T a, T b);
	};

	template <typename T>
	T Math::Max(const T a, const T b)
	{
		return a < b ? b : a;
	}

	template <typename T>
	T Math::Min(T a, T b)
	{
		return a > b ? b : a;
	}
}
