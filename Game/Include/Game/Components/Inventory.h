#pragma once
#include "StackVector.h"

namespace game
{
	constexpr size_t MAX_COPIES_CARD_IN_DECK = 3;

	struct InventorySlot final
	{
		size_t index = SIZE_MAX;
		size_t amount = 1;
	};

	template <size_t S>
	struct Inventory final : jlb::StackVector<InventorySlot, S>
	{
		void Erase(size_t index) override;
	private:
		[[nodiscard]] InventorySlot& OnAdd(InventorySlot& value) override;
	};

	template <size_t S>
	void Inventory<S>::Erase(const size_t index)
	{
		const size_t count = jlb::StackVector<InventorySlot, S>::GetCount();
		assert(count >= 1);
		for (size_t i = index + 1; i < count; ++i)
			jlb::StackVector<InventorySlot, S>::operator[](i - 1) = jlb::StackVector<InventorySlot, S>::operator[](i);
		jlb::StackVector<InventorySlot, S>::SetCount(count - 1);
	}

	template <size_t S>
	InventorySlot& Inventory<S>::OnAdd(InventorySlot& value)
	{
		const size_t count = jlb::StackVector<InventorySlot, S>::GetCount();
		assert(count < S);

		jlb::StackVector<InventorySlot, S>::SetCount(count + 1);
		const auto& idx = value.index;
		size_t i = count;
		bool placed = false;

		while(i > 0 && !placed)
		{
			--i;
			const auto& src = jlb::StackVector<InventorySlot, S>::operator[](i);
			auto& dst = jlb::StackVector<InventorySlot, S>::operator[](i + 1);
			placed = src.index < idx;
			dst = placed ? dst : src;
		}

		return jlb::StackVector<InventorySlot, S>::operator[](i) = value;
	}
}
