#pragma once

namespace jlb
{
	class Math final
	{
	public:
		[[nodiscard]] static float Clamp(float clampable, float min, float max);

		template <typename T>
		[[nodiscard]] static T Max(T a, T b);
	};

	template <typename T>
	T Math::Max(T a, T b)
	{
		return a < b ? b : a;
	}
}
