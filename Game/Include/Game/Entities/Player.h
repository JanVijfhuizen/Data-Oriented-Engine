#pragma once
#include "StackArray.h"
#include "Components/Deck.h"
#include "Systems/MenuSystem.h"

namespace game
{
	constexpr Inventory<> CreatePlayerStarterDeck()
	{
		// Test.
		Inventory deck{};
		auto& fireball = deck.slots[0];
		fireball.index = 0;
		fireball.amount = 0;
		auto& fireball2 = deck.slots[1];
		fireball2.index = 1;
		fireball2.amount = 0;
		auto& fireball3 = deck.slots[2];
		fireball3.index = 2;
		fireball3.amount = 0;
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
		Inventory<> inventory = CreatePlayerStarterDeck();

		MenuUpdateInfo menuUpdateInfo{};
		jlb::StackArray<size_t, 8> menuInteractIds{};
		MenuUpdateInfo deckMenuUpdateInfo{};
		jlb::StackArray<size_t, 8> deckMenuInteractIds{};
		MenuIndex menuIndex = MenuIndex::main;
	};
}
