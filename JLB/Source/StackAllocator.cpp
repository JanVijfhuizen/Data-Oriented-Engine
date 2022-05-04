﻿#include "StackAllocator.h"
#include "JlbMath.h"

namespace jlb
{
	void StackAllocator::Allocate(const CreateInfo& info)
	{
		_data = reinterpret_cast<size_t*>(info.data);
		if (!_data)
			_data = new size_t[ToChunkSize(info.pageSize)];
		_size = ToChunkSize(info.pageSize);
	}

	void StackAllocator::Free() const
	{
		if(_next)
			_next->Free();
		delete[] _data;
	}

	StackAllocator::Allocation<void> StackAllocator::Malloc(const size_t size)
	{
		const size_t chunkSize = ToChunkSize(size);

		if(_size - _current < chunkSize + 1)
		{
			if (!_next)
			{
				_next = new StackAllocator;
				CreateInfo createInfo{};
				createInfo.pageSize = Math::Max(chunkSize + 1, _size);
				_next->Allocate(createInfo);
			}

			return _next->Malloc(size);
		}

		Allocation<void> allocation{};
		allocation.ptr = &_data[_current];
		allocation.id = _id++;
		allocation.src = _data;
		_current += chunkSize + 1;
		_data[_current - 1] = chunkSize + 1;
		return allocation;
	}

	size_t StackAllocator::ToChunkSize(size_t size)
	{
		return size / sizeof(size_t) + (size % sizeof(size_t) > 0);
	}
}
