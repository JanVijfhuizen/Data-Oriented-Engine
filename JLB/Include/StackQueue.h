#pragma once
#include <cassert>
#include <cstdint>

namespace jlb
{
	template <typename T, size_t S>
	class StackQueue final
	{
	public:
		void Enqueue(const T& instance);
		[[nodiscard]] T Peek();
		[[nodiscard]] T Pop();
		[[nodiscard]] size_t GetCount() const;
		[[nodiscard]] static size_t GetLength();

	private:
		T _data[S];
		size_t _count = 0;
		int32_t _head = -1;
	};

	template <typename T, size_t S>
	void StackQueue<T, S>::Enqueue(const T& instance)
	{
		assert(GetCount() < GetLength());
		_head = (_head + 1) % S;
		_data[_head] = instance;
		++_count;
	}

	template <typename T, size_t S>
	T StackQueue<T, S>::Peek()
	{
		assert(GetCount() > 0);
		return _data[_head];
	}

	template <typename T, size_t S>
	T StackQueue<T, S>::Pop()
	{
		assert(GetCount() > 0);
		T&& instance = _data[_head];
		_head = (_head - 1) % S;
		--_count;
		return instance;
	}

	template <typename T, size_t S>
	size_t StackQueue<T, S>::GetCount() const
	{
		return _count;
	}

	template <typename T, size_t S>
	size_t StackQueue<T, S>::GetLength()
	{
		return S;
	}
}
