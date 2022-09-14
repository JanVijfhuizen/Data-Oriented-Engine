#pragma once
#include <vcruntime_typeinfo.h>
#include "Heap.h"
#include "Map.h"
#include "NestedVector.h"
#include "Tuple.h"

namespace jlb
{
	template <typename ...Args>
	class Archetype final
	{
	public:
		void Allocate(StackAllocator& levelAllocator, size_t capacity);

		template <typename ...Components>
		void GetView(const NestedVector<Components>*&... outViews);

		size_t Add(StackAllocator& levelAllocator, Tuple<Args...> components = {});
		void RemoveAt(size_t index);

		[[nodiscard]] size_t GetCount() const;

	private:
		Map<void*> _columns{};
		size_t _nestedCapacity = 0;
		size_t _count = 0;

		template <typename Head, typename Second, typename ...Tail>
		void AllocateColumn(StackAllocator& levelAllocator, size_t nestedCapacity);
		template <typename T>
		void AllocateColumn(StackAllocator& levelAllocator, size_t nestedCapacity);

		template <typename Head, typename Second, typename ...Tail>
		void GetViewColumn(const NestedVector<Head>*& outHead, const NestedVector<Second>*& outSecond, const NestedVector<Tail>*&... outTail);
		template <typename T>
		void GetViewColumn(const NestedVector<T>*& outView);

		template <size_t I, typename Head, typename ...Tail>
		size_t AddInstance(StackAllocator& levelAllocator, Tuple<Args...>& components);

		template <typename Head, typename Second, typename ...Tail>
		void RemoveInstance(size_t index);
		template <typename T>
		void RemoveInstance(size_t index);
	};

	template <typename ... Args>
	void Archetype<Args...>::Allocate(StackAllocator& levelAllocator, size_t capacity)
	{
		_columns.Allocate(levelAllocator, sizeof...(Args));
		AllocateColumn<Args...>(levelAllocator, capacity);
		_nestedCapacity = capacity;
	}

	template <typename ... Args>
	template <typename ... Components>
	void Archetype<Args...>::GetView(const NestedVector<Components>*&... outViews)
	{
		GetViewColumn(outViews...);
	}

	template <typename ... Args>
	size_t Archetype<Args...>::Add(StackAllocator& levelAllocator, Tuple<Args...> components)
	{
		_count++;
		return AddInstance<0, Args...>(levelAllocator, components);
	}

	template <typename ... Args>
	void Archetype<Args...>::RemoveAt(const size_t index)
	{
		_count--;
		RemoveInstance<Args...>(index);
	}

	template <typename ... Args>
	size_t Archetype<Args...>::GetCount() const
	{
		return _count;
	}

	template <typename ... Args>
	template <typename Head, typename Second, typename ... Tail>
	void Archetype<Args...>::AllocateColumn(StackAllocator& levelAllocator, size_t nestedCapacity)
	{
		AllocateColumn<Head>(levelAllocator, nestedCapacity);
		AllocateColumn<Second, Tail...>(levelAllocator, nestedCapacity);
	}

	template <typename ... Args>
	template <typename T>
	void Archetype<Args...>::AllocateColumn(StackAllocator& levelAllocator, size_t nestedCapacity)
	{
		auto ptr = levelAllocator.New<NestedVector<T>>().ptr;
		ptr->Allocate(levelAllocator, nestedCapacity, nestedCapacity);
		_columns.Insert(ptr, typeid(T).hash_code());
	}

	template <typename ... Args>
	template <typename Head, typename Second, typename ... Tail>
	void Archetype<Args...>::GetViewColumn(const NestedVector<Head>*& outHead, const NestedVector<Second>*& outSecond,
		const NestedVector<Tail>*&... outTail)
	{
		GetViewColumn(outHead);
		GetViewColumn(outSecond, outTail...);
	}

	template <typename ... Args>
	template <typename T>
	void Archetype<Args...>::GetViewColumn(const NestedVector<T>*& outView)
	{
		void** column = _columns.Contains(typeid(T).hash_code());
		assert(column);
		outView = static_cast<NestedVector<T>*>(*column);
	}

	template <typename ... Args>
	template <size_t I, typename Head, typename ... Tail>
	size_t Archetype<Args...>::AddInstance(StackAllocator& levelAllocator, Tuple<Args...>& components)
	{
		void** column = _columns.Contains(typeid(Head).hash_code());
		auto& nestedVector = *static_cast<NestedVector<Head>*>(*column);
		nestedVector.Add(levelAllocator, Get<I>(components));
		const size_t count = nestedVector.GetCount();
		if constexpr (sizeof...(Args) > I + 1)
			AddInstance<I + 1, Tail...>(levelAllocator, components);
		return count;
	}

	template <typename ... Args>
	template <typename Head, typename Second, typename ... Tail>
	void Archetype<Args...>::RemoveInstance(const size_t index)
	{
		RemoveInstance<Head>(index);
		RemoveInstance<Second, Tail...>(index);
	}

	template <typename ... Args>
	template <typename T>
	void Archetype<Args...>::RemoveInstance(const size_t index)
	{
		void** column = _columns.Contains(typeid(T).hash_code());
		auto& nestedVector = *static_cast<NestedVector<T>*>(*column);
		nestedVector.RemoveAt(index);
	}
}
