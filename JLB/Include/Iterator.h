#pragma once
#include <cassert>

namespace jlb
{
	// Standard iterator for data containers like vectors/arrays.
	template <typename T>
	struct Iterator final
	{
		// Start of iteration.
		T* memory = nullptr;
		// Length of iteration.
		size_t length = 0;
		// Current index of iteration (relative to begin).
		size_t index = 0;

		T& operator*() const;
		T& operator->() const;

		const Iterator& operator++();
		Iterator operator++(int);

		friend bool operator==(const Iterator& a, const Iterator& b)
		{
			return a.index == b.index;
		}

		friend bool operator!= (const Iterator& a, const Iterator& b)
		{
			return !(a == b);
		}
	};

	template <typename T>
	T& Iterator<T>::operator*() const
	{
		assert(memory);
		assert(index <= length);
		return memory[index];
	}

	template <typename T>
	T& Iterator<T>::operator->() const
	{
		assert(memory);
		assert(index <= length);
		return memory[index];
	}

	template <typename T>
	const Iterator<T>& Iterator<T>::operator++()
	{
		++index;
		return *this;
	}

	template <typename T>
	Iterator<T> Iterator<T>::operator++(int)
	{
		Iterator temp(memory, length, index);
		++index;
		return temp;
	}
}