#pragma once
#include "Array.h"
#include "ArrayUtils.h"

namespace jlb
{
	// Unordered vector.
	template <typename T>
	class Vector : public Array<T>
	{
	public:
		[[nodiscard]] Iterator<T> end() const override;

		void Free(StackAllocator& allocator) override;

		virtual T& Add(const T& value);
		virtual T& Add(const T&& value = {});
		virtual void RemoveAt(size_t index);

		[[nodiscard]] size_t GetCount() const;
		// Cannot exceed the capacity of the managed memory.
		void SetCount(size_t count);

		ArrayView<T> GetView() const override;

	private:
		// The amount of values in this vector.
		size_t _count = 0;

		[[nodiscard]] T& _Add(const T& value);
	};

	template <typename T>
	Iterator<T> Vector<T>::end() const
	{
		return GetView().end();
	}

	template <typename T>
	void Vector<T>::Free(StackAllocator& allocator)
	{
		Array<T>::Free(allocator);
		_count = 0;
	}

	template <typename T>
	T& Vector<T>::Add(const T& value)
	{
		return _Add(value);
	}

	template <typename T>
	T& Vector<T>::Add(const T&& value)
	{
		return _Add(value);
	}

	template <typename T>
	void Vector<T>::RemoveAt(const size_t index)
	{
		// Swap the removed value with the last instance.
		Swap(GetView(), index, _count - 1);
		_count--;
	}

	template <typename T>
	void Vector<T>::SetCount(const size_t count)
	{
		assert(count <= Array<T>::GetLength());
		_count = count;
	}

	template <typename T>
	ArrayView<T> Vector<T>::GetView() const
	{
		auto view = Array<T>::GetView();
		view.length = _count;
		return view;
	}

	template <typename T>
	size_t Vector<T>::GetCount() const
	{
		return _count;
	}

	template <typename T>
	T& Vector<T>::_Add(const T& value)
	{
		assert(_count + 1 <= Array<T>::GetLength());
		++_count;
		return GetView()[_count - 1] = value;
	}
}
