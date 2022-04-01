#pragma once
#include "StackArray.h"

namespace jlb
{
	/// <summary>
	/// An unsorted vector that operates on the stack, rather than on the heap.
	/// </summary>
	template <typename T, size_t S>
	struct StackVector : StackArray<T, S>
	{
		/// <summary>
		/// Add object to the vector.
		/// </summary>
		/// <param name="value"></param>
		/// <returns>Reference to the added object.</returns>
		T& Add(T& value);
		/// <summary>
		/// Add object to the vector.
		/// </summary>
		/// <returns>Reference to the added object.</returns>
		T& Add(T&& value);
		/// <summary>
		/// Removes object at the given index.
		/// </summary>
		void Erase(size_t index);
		/// <summary>
		/// Returns the amount of objects in the vector.
		/// </summary>
		[[nodiscard]] size_t GetCount() const;

		Iterator<T> end() override;

	private:
		size_t _count = 0;

		T& _Add(T& value);
	};

	template <typename T, size_t S>
	T& StackVector<T, S>::Add(T& value)
	{
		return _Add(value);
	}

	template <typename T, size_t S>
	T& StackVector<T, S>::Add(T&& value)
	{
		return _Add(value);
	}

	template <typename T, size_t S>
	void StackVector<T, S>::Erase(const size_t index)
	{
		assert(index < _count);
		StackArray<T, S>::Swap(index, _count--);
	}

	template <typename T, size_t S>
	T& StackVector<T, S>::_Add(T& value)
	{
		return StackArray<T, S>::operator[](_count++) = value;
	}

	template <typename T, size_t S>
	size_t StackVector<T, S>::GetCount() const
	{
		return _count;
	}

	template <typename T, size_t S>
	Iterator<T> StackVector<T, S>::end()
	{
		Iterator<T> it;
		it.memory = StackArray<T, S>::GetData();
		it.index = _count;
		it.length = _count;
		return it;
	}
}
