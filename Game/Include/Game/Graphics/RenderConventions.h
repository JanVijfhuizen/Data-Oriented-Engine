#pragma once

namespace game::renderConventions
{
	constexpr size_t ENTITY_SIZE = 16;
	constexpr size_t TEXT_SIZE = 6;
	constexpr size_t CURSOR_SIZE = 4;

	constexpr size_t ENTITY_HEAD_SIZE = 6;
	constexpr size_t ENTITY_TORSO_SIZE = 6;
	constexpr size_t ENTITY_ARM_SIZE = 4;
	constexpr size_t ENTITY_LEG_SIZE = 4;

	constexpr glm::ivec2 ENTITY_HEAD_POS{5, 0};
	constexpr glm::ivec2 ENTITY_TORSO_POS = ENTITY_HEAD_POS + glm::ivec2(0, ENTITY_HEAD_SIZE);
	constexpr glm::ivec2 ENTITY_L_ARM_POS{ 0, 5 };
	constexpr glm::ivec2 ENTITY_L_LEG_POS = ENTITY_TORSO_POS + glm::ivec2(-1, ENTITY_TORSO_SIZE);

	enum TextureAtlasLocations
	{
		Player,
		Sword,
		Cursor
	};
}