#pragma once
#include <glm/vec2.hpp>

namespace jlb
{
	template <typename T = int32_t>
	struct TBounds final
	{
		typedef glm::vec<2, T, glm::defaultp> Vec2;

		Vec2 lBot{};
		Vec2 rTop{};
		uint32_t layers = 1;

		TBounds();
		TBounds(const Vec2& position, const Vec2& scale = Vec2{0});

		[[nodiscard]] Vec2 GetCenter() const;
	};

	typedef TBounds<int32_t> Bounds;
	typedef TBounds<float> FBounds;

	template <typename T>
	TBounds<T>::TBounds() = default;

	template <typename T>
	TBounds<T>::TBounds(const Vec2& position, const Vec2& scale) : lBot(position), rTop(position)
	{
		lBot -= scale / 2;
		rTop += scale / 2;
	}

	template <typename T>
	typename TBounds<T>::Vec2 TBounds<T>::GetCenter() const
	{
		return lBot + (rTop - lBot) / 2;
	}
}
