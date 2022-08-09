#pragma once

namespace game
{
	constexpr size_t MAX_COPIES_CARD_IN_DECK = 3;

	struct InventorySlot final
	{
		size_t index = SIZE_MAX;
		size_t amount = 0;
	};

	struct Inventory final
	{
		jlb::ArrayView<InventorySlot> src{};

		[[nodiscard]] InventorySlot& operator[](size_t i) const;

		InventorySlot& Insert(size_t index);
		void Erase(size_t index);

		[[nodiscard]] size_t GetCount() const;
		[[nodiscard]] size_t GetLength() const;
		[[nodiscard]] jlb::Iterator<InventorySlot> begin() const;
		[[nodiscard]] jlb::Iterator<InventorySlot> end() const;
	private:
		size_t _count = 0;
	};

	inline InventorySlot& Inventory::operator[](const size_t i) const
	{
		return src[i];
	}

	inline size_t Inventory::GetCount() const
	{
		return _count;
	}

	inline size_t Inventory::GetLength() const
	{
		return src.length;
	}

	inline jlb::Iterator<InventorySlot> Inventory::begin() const
	{
		return src.begin();
	}

	inline jlb::Iterator<InventorySlot> Inventory::end() const
	{
		return src.end();
	}

	inline InventorySlot& Inventory::Insert(const size_t index)
	{
		assert(_count < src.length);
		++_count;

		size_t i = _count;
		bool placed = false;

		while (i > 0 && !placed)
		{
			--i;
			const auto& a = src[i];
			auto& b = src[i + 1];
			placed = a.index < index || a.index == SIZE_MAX;
			b = a;
		}

		auto& slot = src[i] = {};
		slot.index = index;
		return slot;
	}

	inline void Inventory::Erase(const size_t index)
	{
		assert(_count >= 1);
		for (size_t i = index + 1; i < _count; ++i)
			src[i - 1] = src[i];
		--_count;
	}
}
