#pragma once

namespace game
{
	constexpr size_t MAX_COPIES_CARD_IN_DECK = 3;

	struct InventorySlot final
	{
		size_t index = SIZE_MAX;
		size_t amount = 1;
	};

	template <size_t S>
	struct Inventory final
	{
		[[nodiscard]] operator jlb::ArrayView<InventorySlot>();

		InventorySlot slots[S];
		size_t count = 0;

		[[nodiscard]] static size_t GetLength();
	};
	
	template <size_t S>
	Inventory<S>::operator jlb::ArrayView<InventorySlot>()
	{
		jlb::ArrayView<InventorySlot> view{};
		view.length = count;
		view.data = slots;
		return view;
	}

	template <size_t S>
	size_t Inventory<S>::GetLength()
	{
		return S;
	}
}
