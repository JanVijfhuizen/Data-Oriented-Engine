#pragma once
#include "ArrayView.h"

namespace jlb
{
	/// <summary>
	/// An array that operates on the stack, rather than on the heap.
	/// </summary>
	template <typename T, size_t S>
	struct StackArray
	{
		// ReSharper disable once CppNonExplicitConvertingConstructor
		StackArray(const T& fillValue = {});
		// ReSharper disable once CppNonExplicitConvertingConstructor
		StackArray(T* src);
		virtual ~StackArray() = default;

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

		[[nodiscard]] virtual T& operator[](size_t index);
		[[nodiscard]] Iterator<T> begin();
		[[nodiscard]] Iterator<T> end();
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
	void StackArray<T, S>::Swap(size_t a, size_t b)
	{
		assert(a < S && b < S);
		const T temp = _memory[a];
		_memory[a] = _memory[b];
		_memory[b] = temp;
	}

	template <typename T, size_t S>
	void StackArray<T, S>::Copy(size_t begin, size_t end, T* src)
	{
		memcpy(&_memory[begin], src, (end - begin) * sizeof(T));
	}

	template <typename T, size_t S>
	T* StackArray<T, S>::GetData() const
	{
		return _memory;
	}

	template <typename T, size_t S>
	T& StackArray<T, S>::operator[](const size_t index)
	{
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
