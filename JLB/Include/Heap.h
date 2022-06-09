#pragma once
#include <cassert>
#include "Array.h"
#include "KeyPair.h"

namespace jlb
{
	/// <summary>
	/// Binary tree that can be used to quickly sort data based on the key value.
	/// </summary>
	template <typename T>
	class Heap
	{
	public:
		// Function used to get a hash value from a value, which is used to sort values.
		size_t(*hasher)(T& value);

		void Allocate(StackAllocator& allocator, size_t size, const KeyPair<T>& fillValue = {});
		void Free(StackAllocator& allocator);

		void Insert(T& value, size_t key = SIZE_MAX);
		void Insert(T&& value, size_t key = SIZE_MAX);

		// Get a copy of the top value in the heap.
		[[nodiscard]] T Peek();
		// Returns and removes the top value of the Heap.
		T Pop();

		void Clear();
		[[nodiscard]] size_t GetCount() const;

	private:
		size_t _count = 0;
		Array<KeyPair<T>> _array{};

		void _Insert(T& value, size_t key = SIZE_MAX);
		void HeapifyBottomToTop(uint32_t index);
		void HeapifyTopToBottom(uint32_t index);
	};

	template <typename T>
	void Heap<T>::Allocate(StackAllocator& allocator, const size_t size, const KeyPair<T>& fillValue)
	{
		_array.Allocate(allocator, size + 1, fillValue);
	}

	template <typename T>
	void Heap<T>::Free(StackAllocator& allocator)
	{
		_array.Free(allocator);
	}

	template <typename T>
	void Heap<T>::Insert(T& value, const size_t key)
	{
		_Insert(value, key);
	}

	template <typename T>
	void Heap<T>::Insert(T&& value, const size_t key)
	{
		_Insert(value, key);
	}

	template <typename T>
	void Heap<T>::_Insert(T& value, const size_t key)
	{
		_count++;
		assert(_count < _array.GetLength());
		const auto data = _array.GetData();

		auto& keyPair = data[_count];
		keyPair.key = key == SIZE_MAX ? hasher(value) : key;
		keyPair.value = value;
		HeapifyBottomToTop(_count);
	}

	template <typename T>
	T Heap<T>::Peek()
	{
		assert(_count > 0);
		const auto data = _array.GetData();
		const T value = data[1].value;
		return value;
	}

	template <typename T>
	T Heap<T>::Pop()
	{
		assert(_count > 0);

		const auto data = _array.GetData();
		const T value = data[1].value;
		data[1] = data[_count--];

		HeapifyTopToBottom(1);
		return value;
	}

	template <typename T>
	void Heap<T>::Clear()
	{
		_count = 0;
	}

	template <typename T>
	size_t Heap<T>::GetCount() const
	{
		return _count;
	}

	template <typename T>
	void Heap<T>::HeapifyBottomToTop(const uint32_t index)
	{
		// Tree root found.
		if (index <= 1)
			return;

		const auto data = _array.GetData();
		uint32_t parentIndex = index / 2;

		// If current is smaller than the parent, swap and continue.
		if (data[index].key < data[parentIndex].key)
		{
			Swap(_array.GetView(), index, parentIndex);
			HeapifyBottomToTop(parentIndex);
		}
	}

	template <typename T>
	void Heap<T>::HeapifyTopToBottom(const uint32_t index)
	{
		const uint32_t left = index * 2;
		const uint32_t right = index * 2 + 1;

		// If no more nodes remain on the left side.
		if (_count < left)
			return;

		const auto data = Array<KeyPair<T>>::GetData();
		// Is the left node smaller than index.
		const bool lDiff = data[index].key > data[left].key;
		// Is the right node smaller than index.
		const bool rDiff = _count > left ? data[index].key > data[right].key : false;
		// Is left smaller than right.
		const bool dir = rDiff ? data[left].key > data[right].key : false;

		if (lDiff || rDiff)
		{
			const uint32_t newIndex = left + dir;
			Swap(_array, newIndex, index);
			HeapifyTopToBottom(newIndex);
		}
	}
}
