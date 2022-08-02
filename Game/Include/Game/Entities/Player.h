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
		fireball.amount = MAX_COPIES_CARD_IN_DECK - 1;
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
		Deck<> deck = CreatePlayerStarterDeck();

		MenuUpdateInfo menuUpdateInfo{};
		jlb::StackArray<size_t, 8> menuInteractIds{};
		MenuUpdateInfo deckMenuUpdateInfo{};
		jlb::StackArray<size_t, 8> deckMenuInteractIds{};
		MenuIndex menuIndex = MenuIndex::main;
	};
}
