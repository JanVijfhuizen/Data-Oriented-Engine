#pragma once
#include "Iterator.h"

namespace jlb
{
	/// <summary>
	/// Simple struct that allows a chunk of memory to function as an array.
	/// </summary>
	template <typename T>
	struct ArrayView
	{
		T* data;
		size_t length;

		ArrayView() = default;
		// ReSharper disable once CppNonExplicitConvertingConstructor
		ArrayView(T& singleValue);
		ArrayView(T* data, size_t length);

		[[nodiscard]] T& operator[](size_t index);

		[[nodiscard]] Iterator<T> begin();
		[[nodiscard]] Iterator<T> end();
	};

	template <typename T>
	ArrayView<T>::ArrayView(T& singleValue) : data(&singleValue), length(1)
	{
		
	}

	template <typename T>
	ArrayView<T>::ArrayView(T* data, const size_t length) : data(data), length(length)
	{

	}

	template <typename T>
	T& ArrayView<T>::operator[](const size_t index)
	{
		assert(index < length);
		return data[index];
	}

	template <typename T>
	Iterator<T> ArrayView<T>::begin()
	{
		Iterator<T> it;
		it.memory = data;
		it.index = 0;
		it.length = length;
		return it;
	}

	template <typename T>
	Iterator<T> ArrayView<T>::end()
	{
		Iterator<T> it;
		it.memory = data;
		it.index = length;
		it.length = length;
		return it;
	}
}
