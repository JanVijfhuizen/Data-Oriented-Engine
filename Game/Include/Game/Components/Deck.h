#pragma once

namespace game
{
	constexpr size_t MAX_CARDS_IN_DECK = 16;
	constexpr size_t MAX_COPIES_CARD_IN_DECK = 3;

	struct InventorySlot final
	{
		size_t index = SIZE_MAX;
		size_t amount = 1;
	};

	template <size_t S = MAX_CARDS_IN_DECK>
	struct Inventory final
	{
		[[nodiscard]] operator jlb::ArrayView<InventorySlot>();

		InventorySlot slots[S];
		size_t count = 0;
	};

	template <size_t S>
	Inventory<S>::operator jlb::ArrayView<InventorySlot>()
	{
		jlb::ArrayView<InventorySlot> view{};
		view.length = count;
		view.data = slots;
		return view;
	}
}
