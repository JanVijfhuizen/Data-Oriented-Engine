#pragma once
#include "StackArray.h"
#include "Components/Inventory.h"
#include "Systems/MenuSystem.h"

namespace game
{
	constexpr size_t PLAYER_INVENTORY_SIZE = 32;

	constexpr Inventory<PLAYER_INVENTORY_SIZE> CreatePlayerStarterDeck()
	{
		// Test.
		Inventory<PLAYER_INVENTORY_SIZE> deck{};
		auto& fireball = deck.slots[0];
		fireball.index = 0;
		fireball.amount = 0;
		auto& root = deck.slots[1];
		root.index = 1;
		root.amount = 0;
		auto& bash = deck.slots[2];
		bash.index = 2;
		bash.amount = 1;
		deck.count = 3;
		return deck;
	}

	struct Player final
	{
		enum class MenuIndex
		{
			main,
			cards
		};

		Character character{};
		Inventory<PLAYER_INVENTORY_SIZE> inventory = CreatePlayerStarterDeck();

		MenuUpdateInfo menuUpdateInfo{};
		jlb::StackArray<size_t, 8> menuInteractIds{};
		MenuUpdateInfo deckMenuUpdateInfo{};
		jlb::StackArray<size_t, 8> deckMenuInteractIds{};
		MenuIndex menuIndex = MenuIndex::main;
		size_t cardHovered = SIZE_MAX;
	};
}
