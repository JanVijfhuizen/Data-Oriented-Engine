#pragma once
#include <cassert>
#include "StackAllocator.h"
#include "ArrayView.h"
#include <cstring>
#include <cstdint>

namespace jlb
{
	/// <summary>
	/// Array that does not have ownership over the memory that it uses.
	/// </summary>
	template <typename T>
	class Array
	{
	public:
		[[nodiscard]] virtual T& operator[](size_t index);
		[[nodiscard]] size_t GetLength() const;

		/// <summary>
		/// Allocates a chunk of memory to be managed.<br>
		/// The view does not own this memory, and does not free any memory (previously) managed.
		/// </summary>
		/// <param name="allocator">Allocator from which to allocate.</param>
		/// <param name="size">Size of the array.</param>
		/// <param name="fillValue">The array will be initialized with this value.</param>
		virtual void Allocate(StackAllocator& allocator, size_t size, const T& fillValue = {});

		/// <summary>
		/// Allocates a chunk of memory to be managed.<br>
		/// The view does not own this memory, and does not free any memory (previously) managed.
		/// </summary>
		/// <param name="allocator">Allocator from which to allocate.</param>
		/// <param name="size">Size of the array.</param>
		/// <param name="src">The data to copy into the array.</param>
		virtual void AllocateAndCopy(StackAllocator& allocator, size_t size, T* src);

		/// <summary>
		/// Frees the array from the linear allocator.
		/// </summary>
		/// <param name="allocator">Allocator to free it from.</param>
		virtual void Free(StackAllocator& allocator);

		/// <summary>
		/// Swaps values at the defined indexes.
		/// </summary>
		/// <param name="a">Index A.</param>
		/// <param name="b">Index B.</param>
		void Swap(size_t a, size_t b);

		/// <summary>
		/// Copy the data from the source into this array.
		/// </summary>
		/// <param name="begin">Begin index.</param>
		/// <param name="end">End index.</param>
		/// <param name="src">Source data.</param>
		void Copy(size_t begin, size_t end, T* src);

		/// <summary>
		/// Get a raw pointer to the managed memory.
		/// </summary>
		[[nodiscard]] T* GetData() const;

		[[nodiscard]] virtual Iterator<T> begin();
		[[nodiscard]] virtual Iterator<T> end();

		[[nodiscard]] operator bool() const;

		[[nodiscard]] operator ArrayView<T>() const;

	private:
		StackAllocator::Allocation<T> _allocation{};
		size_t _length = 0;
	};

	template <typename T>
	T& Array<T>::operator[](const size_t index)
	{
		assert(index < _length);
		return _allocation.ptr[index];
	}

	template <typename T>
	size_t Array<T>::GetLength() const
	{
		return _length;
	}

	template <typename T>
	void Array<T>::Allocate(StackAllocator& allocator, const size_t size, const T& fillValue)
	{
		assert(!_allocation);

		_allocation = allocator.New<T>(size);
		_length = size;

		for (size_t i = 0; i < size; ++i)
			_allocation.ptr[i] = fillValue;
	}

	template <typename T>
	void Array<T>::AllocateAndCopy(StackAllocator& allocator, const size_t size, T* src)
	{
		_allocation = allocator.New<T>(size);
		_length = size;

		memcpy(_allocation.ptr, src, size * sizeof(T));
	}

	template <typename T>
	void Array<T>::Free(StackAllocator& allocator)
	{
		if (!_allocation)
			return;
		allocator.MFree(_allocation);
		_allocation = {};
	}

	template <typename T>
	void Array<T>::Swap(const size_t a, const size_t b)
	{
		assert(a < _length&& b < _length);
		const T temp = _allocation.ptr[a];
		_allocation.ptr[a] = _allocation.ptr[b];
		_allocation.ptr[b] = temp;
	}

	template <typename T>
	void Array<T>::Copy(const size_t begin, const size_t end, T* src)
	{
		memcpy(&_allocation.ptr[begin], src, (end - begin) * sizeof(T));
	}

	template <typename T>
	T* Array<T>::GetData() const
	{
		return _allocation.ptr;
	}

	template <typename T>
	Iterator<T> Array<T>::begin()
	{
		Iterator<T> it;
		it.memory = _allocation.ptr;
		it.index = 0;
		it.length = _length;
		return it;
	}

	template <typename T>
	Iterator<T> Array<T>::end()
	{
		Iterator<T> it;
		it.memory = _allocation.ptr;
		it.index = _length;
		it.length = _length;
		return it;
	}

	template <typename T>
	Array<T>::operator bool() const
	{
		return _length > 0;
	}

	template <typename T>
	Array<T>::operator ArrayView<T>() const
	{
		return ArrayView<T>{_allocation.ptr, _length};
	}
}
