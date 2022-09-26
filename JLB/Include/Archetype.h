#pragma once
#include <vcruntime_typeinfo.h>
#include "Heap.h"
#include "Map.h"
#include "NestedVector.h"
#include "Tuple.h"

namespace jlb
{
	template <typename Info>
	class IArchetype
	{
	public:
		virtual void Allocate(StackAllocator& levelAllocator, size_t capacity);
		virtual void OnUpdate(const Info& info);

		virtual void OnKeyInput(int key, int action);
		virtual void OnMouseInput(int key, int action);
	};

	template <typename T>
	struct ArchetypeView final
	{
		template <typename Info, typename ...Args>
		friend class Archetype;

		[[nodiscard]] T& operator[](size_t index) const;
		[[nodiscard]] typename NestedVector<T>::Iterator begin() const;
		[[nodiscard]] typename NestedVector<T>::Iterator end() const;

	private:
		NestedVector<T>* _nestedVector = nullptr;
	};

	template <typename Info, typename ...Args>
	class Archetype : public IArchetype<Info>
	{
	public:
		typedef Tuple<Args...> Prototype;

		void Allocate(StackAllocator& levelAllocator, size_t capacity) override;

		template <typename ...Components>
		void GetViews(ArchetypeView<Components>&... outViews);

		virtual size_t Add(StackAllocator& levelAllocator, Tuple<Args...> components = {});
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
		void GetViewColumn(ArchetypeView<Head>& outHead, ArchetypeView<Second>& outSecond,
			ArchetypeView<Tail>&... outTail);

		template <typename T>
		void GetViewColumn(ArchetypeView<T>& outView);

		template <size_t I, typename Head, typename ...Tail>
		size_t AddInstance(StackAllocator& levelAllocator, Tuple<Args...>& components);

		template <typename Head, typename Second, typename ...Tail>
		void RemoveInstance(size_t index);

		template <typename T>
		void RemoveInstance(size_t index);
	};

	template <typename T>
	T& ArchetypeView<T>::operator[](const size_t index) const
	{
		return _nestedVector->operator[](index);
	}

	template <typename T>
	typename NestedVector<T>::Iterator ArchetypeView<T>::begin() const
	{
		return _nestedVector->begin();
	}

	template <typename T>
	typename NestedVector<T>::Iterator ArchetypeView<T>::end() const
	{
		return _nestedVector->end();
	}

	template <typename Info>
	void IArchetype<Info>::Allocate(StackAllocator& levelAllocator, size_t capacity)
	{
	}

	template <typename Info>
	void IArchetype<Info>::OnUpdate(const Info& info)
	{
	}

	template <typename Info>
	void IArchetype<Info>::OnKeyInput(int key, int action)
	{
	}

	template <typename Info>
	void IArchetype<Info>::OnMouseInput(int key, int action)
	{
	}

	template <typename Info, typename ... Args>
	void Archetype<Info, Args...>::Allocate(StackAllocator& levelAllocator, const size_t capacity)
	{
		IArchetype<Info>::Allocate(levelAllocator, capacity);

		_columns.Allocate(levelAllocator, sizeof...(Args));
		AllocateColumn<Args...>(levelAllocator, capacity);
		_nestedCapacity = capacity;
	}

	template <typename Info, typename ... Args>
	template <typename ... Components>
	void Archetype<Info, Args...>::GetViews(ArchetypeView<Components>&... outViews)
	{
		GetViewColumn(outViews...);
	}

	template <typename Info, typename ... Args>
	size_t Archetype<Info, Args...>::Add(StackAllocator& levelAllocator, Tuple<Args...> components)
	{
		_count++;
		return AddInstance<0, Args...>(levelAllocator, components);
	}

	template <typename Info, typename ... Args>
	void Archetype<Info, Args...>::RemoveAt(const size_t index)
	{
		_count--;
		RemoveInstance<Args...>(index);
	}

	template <typename Info, typename ... Args>
	size_t Archetype<Info, Args...>::GetCount() const
	{
		return _count;
	}

	template <typename Info, typename ... Args>
	template <typename Head, typename Second, typename ... Tail>
	void Archetype<Info, Args...>::AllocateColumn(StackAllocator& levelAllocator, const size_t nestedCapacity)
	{
		AllocateColumn<Head>(levelAllocator, nestedCapacity);
		AllocateColumn<Second, Tail...>(levelAllocator, nestedCapacity);
	}

	template <typename Info, typename ... Args>
	template <typename T>
	void Archetype<Info, Args...>::AllocateColumn(StackAllocator& levelAllocator, const size_t nestedCapacity)
	{
		auto ptr = levelAllocator.New<NestedVector<T>>().ptr;
		ptr->Allocate(levelAllocator, nestedCapacity, nestedCapacity);
		_columns.Insert(ptr, typeid(T).hash_code());
	}

	template <typename Info, typename ... Args>
	template <typename Head, typename Second, typename ... Tail>
	void Archetype<Info, Args...>::GetViewColumn(ArchetypeView<Head>& outHead, ArchetypeView<Second>& outSecond,
		ArchetypeView<Tail>&... outTail)
	{
		GetViewColumn(outHead);
		GetViewColumn(outSecond, outTail...);
	}

	template <typename Info, typename ... Args>
	template <typename T>
	void Archetype<Info, Args...>::GetViewColumn(ArchetypeView<T>& outView)
	{
		void** column = _columns.Contains(typeid(T).hash_code());
		assert(column);
		outView._nestedVector = static_cast<NestedVector<T>*>(*column);
	}

	template <typename Info, typename ... Args>
	template <size_t I, typename Head, typename ... Tail>
	size_t Archetype<Info, Args...>::AddInstance(StackAllocator& levelAllocator, Tuple<Args...>& components)
	{
		void** column = _columns.Contains(typeid(Head).hash_code());
		auto& nestedVector = *static_cast<NestedVector<Head>*>(*column);
		nestedVector.Add(levelAllocator, Get<I>(components));
		const size_t count = nestedVector.GetCount();
		if constexpr (sizeof...(Args) > I + 1)
			AddInstance<I + 1, Tail...>(levelAllocator, components);
		return count;
	}

	template <typename Info, typename ... Args>
	template <typename Head, typename Second, typename ... Tail>
	void Archetype<Info, Args...>::RemoveInstance(const size_t index)
	{
		RemoveInstance<Head>(index);
		RemoveInstance<Second, Tail...>(index);
	}

	template <typename Info, typename ... Args>
	template <typename T>
	void Archetype<Info, Args...>::RemoveInstance(const size_t index)
	{
		void** column = _columns.Contains(typeid(T).hash_code());
		auto& nestedVector = *static_cast<NestedVector<T>*>(*column);
		nestedVector.RemoveAt(index);
	}
}
