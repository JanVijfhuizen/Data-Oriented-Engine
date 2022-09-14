#pragma once
#include <vcruntime_typeinfo.h>
#include "EntityManager.h"
#include "Heap.h"
#include "Map.h"

namespace jlb
{
	class Archetype final
	{
	public:
		template <typename ...Components>
		void Allocate(StackAllocator& levelAllocator, size_t capacity);

		template <typename ...Components>
		void GetView(Components*&... outViews);

		[[nodiscard]] size_t GetCapacity() const;

	private:
		struct Column final
		{
			void* data = nullptr;
		};

		Map<Column> _columns{};
		size_t _capacity = 0;

		template <typename Head, typename Second, typename ...Tail>
		void AllocateColumn(StackAllocator& levelAllocator, size_t capacity);
		template <typename T>
		void AllocateColumn(StackAllocator& levelAllocator, size_t capacity);

		template <typename Head, typename Second, typename ...Tail>
		void GetViewColumn(Head*& outHead, Second*& outSecond, Tail*&... outTail);
		template <typename T>
		void GetViewColumn(T*& outView);
	};

	inline size_t Archetype::GetCapacity() const
	{
		return _capacity;
	}

	template <typename ...Components>
	void Archetype::Allocate(StackAllocator& levelAllocator, const size_t capacity)
	{
		_columns.Allocate(levelAllocator, sizeof...(Components));
		AllocateColumn<Components...>(levelAllocator, capacity);
		_capacity = capacity;
	}

	template <typename ... Components>
	void Archetype::GetView(Components*&... outViews)
	{
		GetViewColumn(outViews...);
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
		column.data = levelAllocator.Malloc(sizeof(T) * capacity).ptr;
		_columns.Insert(column, typeid(T).hash_code());
	}

	template <typename Head, typename Second, typename ...Tail>
	void Archetype::GetViewColumn(Head*& outHead, Second*& outSecond, Tail*&... outTail)
	{
		GetViewColumn(outHead);
		GetViewColumn(outSecond, outTail...);
	}

	template <typename T>
	void Archetype::GetViewColumn(T*& outView)
	{
		Column* column = _columns.Contains(typeid(T).hash_code());
		assert(column);
		outView = static_cast<T*>(column->data);
	}
}
