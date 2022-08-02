#pragma once

namespace game
{
	struct DeckSlot final
	{
		size_t index = SIZE_MAX;
		size_t amount = 0;
	};

	template <size_t S = 16>
	struct Deck final
	{
		Deck() = default;
		Deck(jlb::ArrayView<DeckSlot> slots)
		{
			
		}


		[[nodiscard]] operator jlb::ArrayView<DeckSlot>();

		DeckSlot slots[S];
	};

	template <size_t S>
	Deck<S>::operator jlb::ArrayView<DeckSlot>()
	{
		size_t length = S;
		for (int32_t i = S - 1; i >= 0; --i)
		{
			const bool empty = slots[i].amount == 0;
			length -= empty;
			i = empty ? i : -1;
		}

		jlb::ArrayView<DeckSlot> view{};
		view.length = length;
		view.data = slots;
		return view;
	}
}
