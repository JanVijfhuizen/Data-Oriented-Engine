#pragma once
#include "StackArray.h"
#include "Components/Deck.h"
#include "Systems/MenuSystem.h"

namespace game
{
	constexpr Deck<> CreatePlayerStarterDeck()
	{
		Deck deck{};
		auto& fireball = deck.slots[0];
		fireball.index = 0;
		fireball.amount = 3;
		deck.count = 1;
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
		MenuUpdateInfo menuUpdateInfo{};
		jlb::StackArray<size_t, 8> menuInteractIds{};
		MenuIndex menuIndex = MenuIndex::main;
		Deck<> deck = CreatePlayerStarterDeck();
	};
}
