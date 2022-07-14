#pragma once
#include "ArrayView.h"

namespace jlb
{
	// An array that operates on the stack, rather than on the heap.
	template <typename T, size_t S>
	struct StackArray
	{
		// ReSharper disable once CppNonExplicitConvertingConstructor
		StackArray(const T& fillValue = {});
		// ReSharper disable once CppNonExplicitConvertingConstructor
		StackArray(T* src);
		virtual ~StackArray() = default;

		[[nodiscard]] T* GetData();
		[[nodiscard]] static size_t GetLength();

		[[nodiscard]] virtual T& operator[](size_t index);
		[[nodiscard]] virtual Iterator<T> begin();
		[[nodiscard]] virtual Iterator<T> end();
		[[nodiscard]] operator ArrayView<T>();

	private:
		T _memory[S];
	};

	template <typename T, size_t S>
	StackArray<T, S>::StackArray(const T& fillValue)
	{
		for (int i = 0; i < S; ++i)
			_memory[i] = fillValue;
	}

	template <typename T, size_t S>
	StackArray<T, S>::StackArray(T* src)
	{
		memcpy(_memory, src, S * sizeof(T));
	}

	template <typename T, size_t S>
	T* StackArray<T, S>::GetData()
	{
		return _memory;
	}

	template <typename T, size_t S>
	size_t StackArray<T, S>::GetLength()
	{
		return S;
	}

	template <typename T, size_t S>
	T& StackArray<T, S>::operator[](const size_t index)
	{
		assert(index < S);
		return _memory[index];
	}

	template <typename T, size_t S>
	Iterator<T> StackArray<T, S>::begin()
	{
		Iterator<T> it;
		it.memory = _memory;
		it.index = 0;
		it.length = S;
		return it;
	}

	template <typename T, size_t S>
	Iterator<T> StackArray<T, S>::end()
	{
		Iterator<T> it;
		it.memory = _memory;
		it.index = S;
		it.length = S;
		return it;
	}

	template <typename T, size_t S>
	StackArray<T, S>::operator ArrayView<T>()
	{
		return ArrayView<T>{_memory, S};
	}
}
