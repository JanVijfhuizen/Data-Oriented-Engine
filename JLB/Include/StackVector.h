#pragma once
#include "StackArray.h"

namespace jlb
{
	// An unsorted vector that operates on the stack, rather than on the heap.
	template <typename T, size_t S>
	struct StackVector : StackArray<T, S>
	{
		T& Add(T& value);
		T& Add(T&& value);
		void Erase(size_t index);

		[[nodiscard]] size_t GetCount() const;
		[[nodiscard]] Iterator<T> end() override;

		[[nodiscard]] operator ArrayView<T>();

	private:
		size_t _count = 0;

		[[nodiscard]] T& _Add(T& value);
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

	template <typename T, size_t S>
	StackVector<T, S>::operator ArrayView<T>()
	{
		ArrayView<T> view{};
		view.data = StackArray<T, S>::GetData();
		view.length = _count;
		return view;
	}
}
