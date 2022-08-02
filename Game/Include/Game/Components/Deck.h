#pragma once

namespace game
{
	constexpr size_t MAX_CARDS_IN_DECK = 16;
	constexpr size_t MAX_COPIES_CARD_IN_DECK = 3;

	struct DeckSlot final
	{
		size_t index = SIZE_MAX;
		size_t amount = 1;
	};

	template <size_t S = MAX_CARDS_IN_DECK>
	struct Deck final
	{
		[[nodiscard]] operator jlb::ArrayView<DeckSlot>();

		DeckSlot slots[S];
		size_t count = 0;
	};

	template <size_t S>
	Deck<S>::operator jlb::ArrayView<DeckSlot>()
	{
		jlb::ArrayView<DeckSlot> view{};
		view.length = count;
		view.data = slots;
		return view;
	}
}
