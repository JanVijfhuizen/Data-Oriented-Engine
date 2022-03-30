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
	class Heap : public Array<KeyPair<T>>
	{
	public:
		// Function used to get a hash value from a value, which is used to sort values.
		size_t(*hasher)(T& value);

		void Allocate(LinearAllocator& allocator, size_t size, const KeyPair<T>& fillValue = {}) override;

		/// <summary>
		/// Inserts a value into the Heap.
		/// </summary>
		/// <param name="value">Value to be inserted.</param>
		/// <param name="key">Used to sort the value. Leave empty to use the hasher.</param>
		void Insert(T& value, size_t key = SIZE_MAX);
		/// <summary>
		/// Inserts a value into the Heap.
		/// </summary>
		/// <param name="value">Value to be inserted.</param>
		/// <param name="key">Used to sort the value. Leave empty to use the hasher.</param>
		void Insert(T&& value, size_t key = SIZE_MAX);
		/// <summary>
		/// Get a copy of the top value in the heap.
		/// </summary>
		[[nodiscard]] T Peek();
		/// <summary>
		/// Returns and removes the top value of the Heap.
		/// </summary>
		T Pop();
		/// <summary>
		/// Sets the count to zero.
		/// </summary>
		void Clear();
		/// <summary>
		/// Gets the amount of values in the Heap.
		/// </summary>
		[[nodiscard]] size_t GetCount() const;

	private:
		size_t _count = 0;

		void _Insert(T& value, size_t key = SIZE_MAX);
		void HeapifyBottomToTop(uint32_t index);
		void HeapifyTopToBottom(uint32_t index);
		void Swap(uint32_t a, uint32_t b);

		KeyPair<T>& operator[](size_t index) override;
		Iterator<KeyPair<T>> begin() override;
		Iterator<KeyPair<T>> end() override;
	};

	template <typename T>
	void Heap<T>::Allocate(LinearAllocator& allocator, const size_t size, const KeyPair<T>& fillValue)
	{
		Array<KeyPair<T>>::Allocate(allocator, size + 1, fillValue);
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
		assert(_count < Array<KeyPair<T>>::GetLength());
		const auto data = Array<KeyPair<T>>::GetData();

		auto& keyPair = data[_count];
		keyPair.key = key == SIZE_MAX ? hasher(value) : key;
		keyPair.value = value;
		HeapifyBottomToTop(_count);
	}

	template <typename T>
	T Heap<T>::Peek()
	{
		assert(_count > 0);
		const auto data = Array<KeyPair<T>>::GetData();
		const T value = data[1].value;
		return value;
	}

	template <typename T>
	T Heap<T>::Pop()
	{
		assert(_count > 0);

		const auto data = Array<KeyPair<T>>::GetData();
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

		const auto data = Array<KeyPair<T>>::GetData();
		uint32_t parentIndex = index / 2;

		// If current is smaller than the parent, swap and continue.
		if (data[index].key < data[parentIndex].key)
		{
			Swap(index, parentIndex);
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
			Swap(newIndex, index);
			HeapifyTopToBottom(newIndex);
		}
	}

	template <typename T>
	void Heap<T>::Swap(const uint32_t a, const uint32_t b)
	{
		const auto data = Array<KeyPair<T>>::GetData();
		KeyPair<T> temp = data[a];
		data[a] = data[b];
		data[b] = temp;
	}

	template <typename T>
	KeyPair<T>& Heap<T>::operator[](const size_t index)
	{
		return Array<KeyPair<T>>::operator[](index);
	}

	template <typename T>
	Iterator<KeyPair<T>> Heap<T>::begin()
	{
		return Array<KeyPair<T>>::begin();
	}

	template <typename T>
	Iterator<KeyPair<T>> Heap<T>::end()
	{
		return Array<KeyPair<T>>::end();
	}
}
