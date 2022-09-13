#pragma once
#include "ArrayView.h"
#include <cstring>

namespace jlb
{
	template <typename T>
	void Swap(ArrayView<T> view, size_t a, size_t b);

	template <typename T>
	void Copy(ArrayView<T> view, size_t begin, size_t end, T* src);

	template <typename T>
	void Swap(const ArrayView<T> view, const size_t a, const size_t b)
	{
		assert(a < view.length&& b < view.length);
		const T temp = view.data[a];
		view.data[a] = view.data[b];
		view.data[b] = temp;
	}

	template <typename T>
	void Copy(const ArrayView<T> view, const size_t begin, const size_t end, T* src)
	{
		assert(end <= view.length && begin < view.length);
		memcpy(&view.data[begin], src, (end - begin) * sizeof(T));
	}
}
