#pragma once

namespace game
{
	struct RenderConventions final
	{
		constexpr static size_t ENTITY_PIXEL_SIZE = 16;
		constexpr static size_t TEXT_PIXEL_SIZE = 6;

		constexpr static float PIXEL_SIZE = 0.008;
		constexpr static float ENTITY_SIZE = PIXEL_SIZE * ENTITY_PIXEL_SIZE;
		constexpr static float TEXT_SIZE = PIXEL_SIZE * TEXT_PIXEL_SIZE;
	};
}