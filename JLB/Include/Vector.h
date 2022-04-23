#pragma once
#include "Array.h"

namespace jlb
{
	/// <summary>
	/// Unordered vector that does not have ownership over the memory that it uses.<br>
	/// It does not resize the capacity automatically.
	/// </summary>
	template <typename T>
	class Vector : public Array<T>
	{
	public:
		/// <summary>
		/// Place a value in the front of the vector and increase it's size by one.<br>
		/// Cannot exceed the capacity of the managed memory.
		/// </summary>
		/// <param name="value">The value to be added to the vector.</param>
		/// <param name="allocator">Add an allocator if you want to be able to dynamically increase the size of the vector.<br>
		/// Only works if the vector was the newest allocation.</param>
		/// <returns>The added value inside the vector.</returns>
		T& Add(T& value, LinearAllocator* allocator = nullptr);
		/// <summary>
		/// Place a value in the front of the vector and increase it's size by one.<br>
		/// Cannot exceed the capacity of the managed memory.
		/// </summary>
		/// <param name="value">The value to be added to the vector.</param>
		/// <param name="allocator">Add an allocator if you want to be able to dynamically increase the size of the vector.<br>
		/// Only works if the vector was the newest allocation.</param>
		/// <returns>The added value inside the vector.</returns>
		T& Add(T&& value = {}, LinearAllocator* allocator = nullptr);
		/// <summary>
		/// Remove the value at a certain index.
		/// </summary>
		/// <param name="index">Index where the value will be removed.</param>
		void RemoveAt(size_t index);
		/// <summary>
		/// Set the count of the vector. Cannot exceed the capacity of the managed memory.
		/// </summary>
		/// <param name="count"></param>
		/// <param name="allocator">Add an allocator if you want to be able to dynamically increase the size of the vector.<br>
		/// Only works if the vector was the newest allocation.</param>
		void SetCount(size_t count, LinearAllocator* allocator = nullptr);
		/// <summary>
		/// Gets the amount of values in the vector.
		/// </summary>
		[[nodiscard]] size_t GetCount() const;
		[[nodiscard]] Iterator<T> end() override;

	private:
		// The amount of values in this vector.
		size_t _count = 0;

		T& _Add(T& value, LinearAllocator* allocator = nullptr);
	};

	template <typename T>
	T& Vector<T>::Add(T& value, LinearAllocator* allocator)
	{
		return _Add(value, allocator);
	}

	template <typename T>
	T& Vector<T>::Add(T&& value, LinearAllocator* allocator)
	{
		return _Add(value, allocator);
	}

	template <typename T>
	void Vector<T>::RemoveAt(const size_t index)
	{
		// Swap the removed value with the last instance.
		Array<T>::Swap(index, --_count);
	}

	template <typename T>
	void Vector<T>::SetCount(const size_t count, LinearAllocator* allocator)
	{
		const bool outOfMemory = count > Array<T>::GetLength();
		if (allocator && outOfMemory)
			Array<T>::Resize(*allocator, count);
		else
			assert(count <= Array<T>::GetLength());
		_count = count;
	}

	template <typename T>
	size_t Vector<T>::GetCount() const
	{
		return _count;
	}

	template <typename T>
	Iterator<T> Vector<T>::end()
	{
		Iterator<T> it;
		it.memory = Array<T>::GetData();
		it.index = _count;
		it.length = Array<T>::GetLength();
		return it;
	}

	template <typename T>
	T& Vector<T>::_Add(T& value, LinearAllocator* allocator)
	{
		const bool outOfMemory = _count + 1 >= Array<T>::GetLength();
		if (allocator && outOfMemory)
			Array<T>::Resize(*allocator, _count + 1);
		else
			assert(outOfMemory);
		return Array<T>::operator[](_count++) = value;
	}
}
