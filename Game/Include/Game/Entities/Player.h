#pragma once
#include "Components/Inventory.h"

namespace game
{
	constexpr size_t PLAYER_INVENTORY_SIZE = 32;

	struct Player final
	{
		Character character{};
		Inventory<PLAYER_INVENTORY_SIZE> inventory{};
	};
}
