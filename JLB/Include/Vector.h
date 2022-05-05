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
		void Free(StackAllocator& allocator) override;

		/// <summary>
		/// Place a value in the front of the vector and increase it's size by one.<br>
		/// Cannot exceed the capacity of the managed memory.
		/// </summary>
		/// <param name="value">The value to be added to the vector.</param>
		/// <returns>The added value inside the vector.</returns>
		T& Add(T& value);
		/// <summary>
		/// Place a value in the front of the vector and increase it's size by one.<br>
		/// Cannot exceed the capacity of the managed memory.
		/// </summary>
		/// <param name="value">The value to be added to the vector.</param>
		/// <returns>The added value inside the vector.</returns>
		T& Add(T&& value = {});
		/// <summary>
		/// Remove the value at a certain index.
		/// </summary>
		/// <param name="index">Index where the value will be removed.</param>
		void RemoveAt(size_t index);
		/// <summary>
		/// Set the count of the vector. Cannot exceed the capacity of the managed memory.
		/// </summary>
		/// <param name="count"></param>
		void SetCount(size_t count);
		/// <summary>
		/// Gets the amount of values in the vector.
		/// </summary>
		[[nodiscard]] size_t GetCount() const;
		[[nodiscard]] Iterator<T> end() override;

	private:
		// The amount of values in this vector.
		size_t _count = 0;

		T& _Add(T& value);
	};

	template <typename T>
	void Vector<T>::Free(StackAllocator& allocator)
	{
		Array<T>::Free(allocator);
		_count = 0;
	}

	template <typename T>
	T& Vector<T>::Add(T& value)
	{
		return _Add(value);
	}

	template <typename T>
	T& Vector<T>::Add(T&& value)
	{
		return _Add(value);
	}

	template <typename T>
	void Vector<T>::RemoveAt(const size_t index)
	{
		// Swap the removed value with the last instance.
		Array<T>::Swap(index, --_count);
	}

	template <typename T>
	void Vector<T>::SetCount(const size_t count)
	{
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
	T& Vector<T>::_Add(T& value)
	{
		assert(_count + 1 <= Array<T>::GetLength());
		return Array<T>::operator[](_count++) = value;
	}
}
