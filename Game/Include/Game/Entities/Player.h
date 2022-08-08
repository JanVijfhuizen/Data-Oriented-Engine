#pragma once
#include "Character.h"
#include "Components/Inventory.h"

namespace game
{
	constexpr size_t PLAYER_INVENTORY_SIZE = 16;

	struct Player final : Character
	{
		Inventory<PLAYER_INVENTORY_SIZE> inventory{};
	};
}
