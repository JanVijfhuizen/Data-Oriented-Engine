#pragma once

namespace jlb
{
	template <typename T, size_t S>
	struct SwapChain final
	{
		[[nodiscard]] T& operator[](size_t index);

		T values[S];
		size_t current = 0;

		void Swap();
		[[nodiscard]] T& GetCurrent();
		[[nodiscard]] T& GetPrevious();
		[[nodiscard]] static size_t GetLength();

		[[nodiscard]] Iterator<T> begin();
		[[nodiscard]] Iterator<T> end();
	};

	template <typename T, size_t S>
	T& SwapChain<T, S>::operator[](const size_t index)
	{
		return values[index];
	}

	template <typename T, size_t S>
	void SwapChain<T, S>::Swap()
	{
		current = (current + 1) % S;
	}

	template <typename T, size_t S>
	T& SwapChain<T, S>::GetCurrent()
	{
		return values[current];
	}

	template <typename T, size_t S>
	T& SwapChain<T, S>::GetPrevious()
	{
		return values[(current - 1) % S];
	}

	template <typename T, size_t S>
	size_t SwapChain<T, S>::GetLength()
	{
		return S;
	}

	template <typename T, size_t S>
	Iterator<T> SwapChain<T, S>::begin()
	{
		Iterator<T> it{};
		it.length = S;
		it.memory = values;
		it.index = 0;
		return it;
	}

	template <typename T, size_t S>
	Iterator<T> SwapChain<T, S>::end()
	{
		Iterator<T> it{};
		it.length = S;
		it.memory = values;
		it.index = S;
		return it;
	}
}
