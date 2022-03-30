#pragma once
#include "Array.h"

namespace jlb
{
	/// <summary>
	/// Data container that operates on a first-in-first-out basis.<br>
	/// Random insertions are not supported, you can only add or remove the newest entries.
	/// </summary>
	template <typename T>
	class Stack : public Array<T>
	{
	public:
		/// <summary>
		/// Add the value on top of the stack.
		/// </summary>
		/// <param name="value">Value to be added.</param>
		/// <returns>Reference to the added value.</returns>
		T& Push(T& value = {});
		/// <summary>
		/// Add the value on top of the stack.
		/// </summary>
		/// <param name="value">Value to be added.</param>
		/// <returns>Reference to the added value.</returns>
		T& Push(T&& value = {});
		/// <summary>
		/// Look at the top value of the stack.
		/// </summary>
		/// <returns>Reference to the value of the stack.</returns>
		[[nodiscard]] T& Peek();
		/// <summary>
		/// Get and remove the top value of the stack.
		/// </summary>
		T Pop();

		/// <summary>
		/// Gets the amount of values in the vector.
		/// </summary>
		[[nodiscard]] size_t GetCount() const;

		Iterator<T> end() override;

	private:
		size_t _count = 0;
	};

	template <typename T>
	T& Stack<T>::Push(T& value)
	{
		return Array<T>::operator[](_count++) = value;
	}

	template <typename T>
	T& Stack<T>::Push(T&& value)
	{
		return Array<T>::operator[](_count++) = value;
	}

	template <typename T>
	T& Stack<T>::Peek()
	{
		return Array<T>::operator[](_count - 1);
	}

	template <typename T>
	T Stack<T>::Pop()
	{
		return Array<T>::operator[](--_count);
	}

	template <typename T>
	size_t Stack<T>::GetCount() const
	{
		return _count;
	}

	template <typename T>
	Iterator<T> Stack<T>::end()
	{
		Iterator<T> it;
		it.memory = Array<T>::GetData();
		it.index = _count;
		it.length = Array<T>::GetLength();
		return it;
	}
}
