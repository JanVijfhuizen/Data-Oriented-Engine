#pragma once
#include <cassert>
#include "StackAllocator.h"
#include "ArrayView.h"
#include <cstring>

namespace jlb
{
	// Array that has ownership of the memory it uses.
	template <typename T>
	class Array
	{
	public:
		virtual T& operator[](size_t index);

		virtual void Allocate(StackAllocator& allocator, size_t size, const T& fillValue = {});
		virtual void Free(StackAllocator& allocator);

		[[nodiscard]] operator bool() const;
		[[nodiscard]] virtual ArrayView<T> GetView() const;
		[[nodiscard]] operator ArrayView<T>() const;

		[[nodiscard]] T* GetData() const;
		[[nodiscard]] size_t GetLength() const;

		[[nodiscard]] virtual Iterator<T> begin() const;
		[[nodiscard]] virtual Iterator<T> end() const;

	private:
		ArrayView<T> _view{};
		AllocationID _allocationId{};
	};

	template <typename T>
	T& Array<T>::operator[](const size_t index)
	{
		return _view[index];
	}

	template <typename T>
	void Array<T>::Allocate(StackAllocator& allocator, const size_t size, const T& fillValue)
	{
		assert(!_allocationId);

		auto allocation = allocator.New<T>(size);
		_allocationId = allocation.id;
		_view = { allocation.ptr, size };

		for (size_t i = 0; i < size; ++i)
			_view.data[i] = fillValue;
	}

	template <typename T>
	void Array<T>::Free(StackAllocator& allocator)
	{
		if (!_allocationId)
			return;
		allocator.MFree(_allocationId);
		_allocationId = {};
	}

	template <typename T>
	Array<T>::operator bool() const
	{
		return _allocationId;
	}

	template <typename T>
	ArrayView<T> Array<T>::GetView() const
	{
		return _view;
	}

	template <typename T>
	Array<T>::operator ArrayView<T>() const
	{
		return GetView();
	}

	template <typename T>
	T* Array<T>::GetData() const
	{
		return _view.data;
	}

	template <typename T>
	size_t Array<T>::GetLength() const
	{
		return _view.length;
	}

	template <typename T>
	Iterator<T> Array<T>::begin() const
	{
		return _view.begin();
	}

	template <typename T>
	Iterator<T> Array<T>::end() const
	{
		return _view.end();
	}
}
