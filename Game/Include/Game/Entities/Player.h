﻿#pragma once
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
		deck.count = 12;

		for (int i = 3; i < 12; ++i)
		{
			deck.slots[i] = deck.slots[0];
		}
		return deck;
	}

	struct Player final
	{
		enum class MenuIndex
		{
			main,
			inventory,
			deck
		};

		Character character{};
		Inventory<PLAYER_INVENTORY_SIZE> inventory = CreatePlayerStarterDeck();

		MenuUpdateInfo menuUpdateInfo{};
		jlb::StackArray<size_t, 6> menuInteractIds{};
		MenuUpdateInfo secondMenuUpdateInfo{};
		jlb::StackArray<size_t, 6> secondMenuInteractIds{};
		MenuIndex menuIndex = MenuIndex::main;
		size_t cardHovered = SIZE_MAX;
		size_t cardActivated = SIZE_MAX;
	};
}
