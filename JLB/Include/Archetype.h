#pragma once
#include <vcruntime_typeinfo.h>
#include "EntityManager.h"
#include "Heap.h"
#include "Map.h"
#include "Node.h"

namespace jlb
{
	template <typename T>
	struct View final
	{
		friend Archetype;

	private:
		Node* node = nullptr;
		size_t capacity = 0;
	};

	class Archetype final
	{
	public:
		template <typename ...Components>
		void Allocate(StackAllocator& levelAllocator, size_t capacity);

		template <typename ...Components>
		void GetView(View<Components>&... outViews);

	private:
		struct Column final
		{
			Node* root = nullptr;
			size_t nodeSize = 0;
		};

		Map<Column> _columns{};
		size_t _capacity = 0;

		template <typename Head, typename Second, typename ...Tail>
		void AllocateColumn(StackAllocator& levelAllocator, size_t capacity);
		template <typename T>
		void AllocateColumn(StackAllocator& levelAllocator, size_t capacity);

		template <typename Head, typename Second, typename ...Tail>
		void GetViewColumn(View<Head>& outHead, View<Second>& outSecond, View<Tail>&... outTail);
		template <typename T>
		void GetViewColumn(View<T>& outView);
	};

	template <typename ...Components>
	void Archetype::Allocate(StackAllocator& levelAllocator, const size_t capacity)
	{
		_columns.Allocate(levelAllocator, sizeof...(Components));
		AllocateColumn<Components...>(levelAllocator, capacity);
		_capacity = capacity;
	}

	template <typename ... Components>
	void Archetype::GetView(View<Components>&... outViews)
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
		column.nodeSize = sizeof(T) * capacity;
		_columns.Insert(column, typeid(T).hash_code());
	}

	template <typename Head, typename Second, typename ...Tail>
	void Archetype::GetViewColumn(View<Head>& outHead, View<Second>& outSecond, View<Tail>&... outTail)
	{
		GetViewColumn(outHead);
		GetViewColumn(outSecond, outTail...);
	}

	template <typename T>
	void Archetype::GetViewColumn(View<T>& outView)
	{
		Column* column = _columns.Contains(typeid(T).hash_code());
		assert(column);
		outView.node = column->root;
		outView.capacity = _capacity;
	}
}
