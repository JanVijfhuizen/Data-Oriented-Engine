#pragma once

namespace game::renderConventions
{
	constexpr size_t ENTITY_SIZE = 16;
	constexpr size_t TEXT_SIZE = 6;
	constexpr size_t CURSOR_SIZE = 4;

	enum TextureAtlasLocations
	{
		Player,
		Sword,
		Cursor
	};
}