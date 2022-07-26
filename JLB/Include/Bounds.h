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
		TBounds(const Vec2& position, Vec2 scale = Vec2{0});

		[[nodiscard]] bool Intersects(const TBounds& other) const;
		[[nodiscard]] bool Intersects(const Vec2& position) const;
		[[nodiscard]] Vec2 GetCenter() const;
	};

	typedef TBounds<int32_t> Bounds;
	typedef TBounds<float> FBounds;

	template <typename T>
	TBounds<T>::TBounds() = default;

	template <typename T>
	TBounds<T>::TBounds(const Vec2& position, Vec2 scale) : lBot(position), rTop(position)
	{
		scale /= 2;
		lBot -= scale;
		rTop += scale;
	}

	template <typename T>
	bool TBounds<T>::Intersects(const TBounds& other) const
	{
		return (layers & other.layers) == 0 ? false : lBot.x <= other.rTop.x && rTop.x >= other.lBot.x && lBot.y <= other.rTop.y && rTop.y >= other.lBot.y;
	}

	template <typename T>
	bool TBounds<T>::Intersects(const Vec2& position) const
	{
		return lBot.x < position.x&& lBot.y < position.y&& rTop.x > position.x&& rTop.y > position.y;
	}

	template <typename T>
	typename TBounds<T>::Vec2 TBounds<T>::GetCenter() const
	{
		return lBot + (rTop - lBot) / 2;
	}
}
