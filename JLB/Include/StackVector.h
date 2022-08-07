#pragma once
#include "ArrayUtils.h"
#include "StackArray.h"

namespace jlb
{
	// An unsorted vector that operates on the stack, rather than on the heap.
	template <typename T, size_t S>
	struct StackVector : StackArray<T, S>
	{
		T& Add(T& value);
		T& Add(T&& value);
		virtual void Erase(size_t index);
		void SetCount(size_t i);

		[[nodiscard]] size_t GetCount() const;
		[[nodiscard]] Iterator<T> end() override;
		[[nodiscard]] ArrayView<T> GetView() override;

	protected:
		[[nodiscard]] virtual T& OnAdd(T& value);
	private:
		size_t _count = 0;
	};

	template <typename T, size_t S>
	T& StackVector<T, S>::Add(T& value)
	{
		return OnAdd(value);
	}

	template <typename T, size_t S>
	T& StackVector<T, S>::Add(T&& value)
	{
		return OnAdd(value);
	}

	template <typename T, size_t S>
	void StackVector<T, S>::Erase(const size_t index)
	{
		assert(index < _count);
		jlb::Swap(GetView(), index, _count--);
	}

	template <typename T, size_t S>
	void StackVector<T, S>::SetCount(const size_t i)
	{
		assert(i < S);
		_count = i;
	}

	template <typename T, size_t S>
	T& StackVector<T, S>::OnAdd(T& value)
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
	ArrayView<T> StackVector<T, S>::GetView()
	{
		ArrayView<T> view{};
		view.data = StackArray<T, S>::GetData();
		view.length = _count;
		return view;
	}
}
