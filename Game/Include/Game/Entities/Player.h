#pragma once
#include "StackArray.h"
#include "Components/Deck.h"
#include "Systems/MenuSystem.h"

namespace game
{
	constexpr Deck<> CreatePlayerStarterDeck()
	{
		Deck deck{};
		for (auto& slot : deck.slots)
		{
			slot.index = 0;
			slot.amount = 1;
		}
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
