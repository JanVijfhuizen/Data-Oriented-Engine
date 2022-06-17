#pragma once
#include "Iterator.h"

namespace jlb
{
	/// <summary>
	/// Simple struct that allows a chunk of memory to function as an array.
	/// Most data structures can be converted into an array view, including singular objects.
	/// </summary>
	template <typename T>
	struct ArrayView
	{
		T* data = nullptr;
		size_t length = 0;

		ArrayView() = default;
		// ReSharper disable once CppNonExplicitConvertingConstructor
		ArrayView(T& instance);
		ArrayView(T* data, size_t length);

		[[nodiscard]] T& operator[](size_t index) const;
		[[nodiscard]] operator bool() const;

		[[nodiscard]] Iterator<T> begin() const;
		[[nodiscard]] Iterator<T> end() const;
	};

	template <typename T>
	ArrayView<T>::ArrayView(T& instance) : data(&instance), length(1)
	{
		
	}

	template <typename T>
	ArrayView<T>::ArrayView(T* data, const size_t length) : data(data), length(length)
	{

	}

	template <typename T>
	ArrayView<T>::operator bool() const
	{
		return data;
	}

	template <typename T>
	T& ArrayView<T>::operator[](const size_t index) const
	{
		assert(index < length);
		return data[index];
	}

	template <typename T>
	Iterator<T> ArrayView<T>::begin() const
	{
		Iterator<T> it;
		it.memory = data;
		it.index = 0;
		it.length = length;
		return it;
	}

	template <typename T>
	Iterator<T> ArrayView<T>::end() const
	{
		Iterator<T> it;
		it.memory = data;
		it.index = length;
		it.length = length;
		return it;
	}
}
