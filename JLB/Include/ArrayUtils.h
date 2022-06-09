#pragma once
#include "ArrayView.h"

namespace jlb
{
	/// Swaps values at the defined indexes in the given array.
	template <typename T>
	void Swap(const ArrayView<T> view, const size_t a, const size_t b)
	{
		assert(a < view.length && b < view.length);
		const T temp = view.data[a];
		view.data[a] = view.data[b];
		view.data[b] = temp;
	}

	template <typename T>
	void Copy(const ArrayView<T> view, const size_t begin, const size_t end, T* src)
	{
		memcpy(&view.data[begin], src, (end - begin) * sizeof(T));
	}
}
