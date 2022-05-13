#pragma once

namespace game
{
	struct RenderConventions final
	{
		constexpr static size_t ENTITY_SIZE = 16;
		constexpr static size_t TEXT_SIZE = 6;
		constexpr static size_t CURSOR_SIZE = 4;

		enum TextureAtlasLocations
		{
			Player,
			Sword,
			Cursor
		};
	};
}