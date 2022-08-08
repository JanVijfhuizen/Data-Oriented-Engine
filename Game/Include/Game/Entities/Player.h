#pragma once
#include "StackArray.h"
#include "Components/Inventory.h"
#include "Systems/MenuSystem.h"

namespace game
{
	constexpr size_t PLAYER_INVENTORY_SIZE = 32;

	struct Player final
	{
		enum class MenuIndex
		{
			main,
			inventory,
			deck
		};

		Character character{};
		Inventory<PLAYER_INVENTORY_SIZE> inventory{};

		MenuUpdateInfo menuUpdateInfo{};
		jlb::StackArray<size_t, 6> menuInteractIds{};
		MenuUpdateInfo secondMenuUpdateInfo{};
		jlb::StackArray<size_t, 6> secondMenuInteractIds{};
		MenuIndex menuIndex = MenuIndex::main;
		size_t cardHovered = SIZE_MAX;
		size_t cardActivated = SIZE_MAX;
	};
}
