#pragma once
#include "Array.h"
#include "ArrayUtils.h"

namespace jlb
{
	// Unordered vector that has ownership over the memory it uses.
	template <typename T>
	class Vector : public Array<T>
	{
	public:
		[[nodiscard]] Iterator<T> end() const override;

		void Free(StackAllocator& allocator) override;

		// Fill in the allocators if you want the vector to dynamically resize.
		virtual T& Add(const T& value, StackAllocator* allocator = nullptr, StackAllocator* tempAllocator = nullptr);
		// Fill in the allocators if you want the vector to dynamically resize.
		virtual T& Add(const T&& value = {}, StackAllocator* allocator = nullptr, StackAllocator* tempAllocator = nullptr);
		virtual void RemoveAt(size_t index);

		[[nodiscard]] size_t GetCount() const;
		// Cannot exceed the capacity of the managed memory.
		void SetCount(size_t count);

		[[nodiscard]] ArrayView<T> GetView() const override;

	private:
		// The amount of values in this vector.
		size_t _count = 0;

		[[nodiscard]] T& _Add(const T& value, StackAllocator* allocator, StackAllocator* tempAllocator);
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
	T& Vector<T>::Add(const T& value, StackAllocator* allocator, StackAllocator* tempAllocator)
	{
		return _Add(value, allocator, tempAllocator);
	}

	template <typename T>
	T& Vector<T>::Add(const T&& value, StackAllocator* allocator, StackAllocator* tempAllocator)
	{
		return _Add(value, allocator, tempAllocator);
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
	T& Vector<T>::_Add(const T& value, StackAllocator* allocator, StackAllocator* tempAllocator)
	{
		const size_t length = Array<T>::GetLength();
		if(_count + 1 > length)
		{
			assert(allocator && tempAllocator);
			assert(allocator->IsOnTop(Array<T>::GetAllocationId()));

			// Create temporary array to store vector data in.
			Array<T> tempArray{};
			tempArray.Allocate(*tempAllocator, length);
			Copy(tempArray.GetView(), 0, length, Array<T>::GetData());

			// Calculate new length with the power of 8.
			size_t newLength = 8;
			while (newLength <= length)
				newLength *= 2;

			// Free data and copy temporary array's values back in.
			Free(*allocator);
			Array<T>::Allocate(*allocator, newLength);
			SetCount(length + 1);
			Copy(GetView(), 0, length, tempArray.GetData());

			// Delete temporary array.
			tempArray.Free(*tempAllocator);
		}

		++_count;
		return Array<T>::GetData()[_count - 1] = value;
	}
}
