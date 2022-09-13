#pragma once
#include <vcruntime_typeinfo.h>
#include "Allocation.h"
#include "Map.h"

namespace jlb
{
	class Archetype final
	{
	public:
		template <typename ...Components>
		void Allocate(StackAllocator& levelAllocator, size_t capacity);

		template <typename ...Components>
		void GetView(Components*&... outArrays);

	private:
		struct Column final
		{
			Allocation<void> alloc{};
		};

		Map<Column> _columns{};

		template <typename Head, typename Second, typename ...Tail>
		void AllocateColumn(StackAllocator& levelAllocator, size_t capacity);
		template <typename T>
		void AllocateColumn(StackAllocator& levelAllocator, size_t capacity);

		template <typename Head, typename Second, typename ...Tail>
		void GetViewColumn(Head*& outHead, Second*& outSecond, Tail*&... outTail);
		template <typename T>
		void GetViewColumn(T*& outArray);
	};

	template <typename ...Components>
	void Archetype::Allocate(StackAllocator& levelAllocator, const size_t capacity)
	{
		_columns.Allocate(levelAllocator, sizeof...(Components));
		AllocateColumn<Components...>(levelAllocator, capacity);
	}

	template <typename ... Components>
	void Archetype::GetView(Components*&... outArrays)
	{
		GetViewColumn(outArrays...);
	}

	template <typename Head, typename Second, typename ... Tail>
	void Archetype::AllocateColumn(StackAllocator& levelAllocator, const size_t capacity)
	{
		AllocateColumn<Head>(levelAllocator, capacity);
		AllocateColumn<Second, Tail...>(levelAllocator, capacity);
	}

	template <typename T>
	void Archetype::AllocateColumn(StackAllocator& levelAllocator, const size_t capacity)
	{
		Column column{};
		column.alloc = levelAllocator.Malloc(sizeof(T) * capacity);
		T* ptr = static_cast<T*>(column.alloc.ptr);
		for (size_t i = 0; i < capacity; ++i)
			ptr[i] = {};
		_columns.Insert(column, typeid(T).hash_code());
	}

	template <typename Head, typename Second, typename ...Tail>
	void Archetype::GetViewColumn(Head*& outHead, Second*& outSecond, Tail*&... outTail)
	{
		GetViewColumn(outHead);
		GetViewColumn(outSecond, outTail...);
	}

	template <typename T>
	void Archetype::GetViewColumn(T*& outArray)
	{
		Column* column = _columns.Contains(typeid(T).hash_code());
		assert(column);
		outArray = static_cast<T*>(column->alloc.ptr);
	}
}
