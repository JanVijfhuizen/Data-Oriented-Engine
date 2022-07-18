#include "StackAllocator.h"
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

	void StackAllocator::Free()
	{
		if (_next)
		{
			_next->Free();
			delete _next;
			_next = nullptr;
		}
		delete[] _data;
	}

	Allocation<void> StackAllocator::Malloc(const size_t size)
	{
		const size_t chunkSize = ToChunkSize(size);

		if(_size - _current < chunkSize + 1)
		{
			if (!_next)
			{
				_next = new StackAllocator;
				CreateInfo createInfo{};
				createInfo.pageSize = math::Max(size + sizeof(size_t), _size * sizeof(size_t));
				_next->Allocate(createInfo);
			}

			return _next->Malloc(size);
		}

		Allocation<void> allocation{};
		allocation.ptr = &_data[_current];
		allocation.id.index = _id++;
		allocation.id.src = _data;
		_current += chunkSize + 1;
		_data[_current - 1] = chunkSize + 1;
		return allocation;
	}

	void StackAllocator::MFreeUnsafe(size_t* src)
	{
		if (_data != src)
		{
			_next->MFreeUnsafe(src);
			return;
		}

		const size_t size = _data[_current - 1];
		_current -= size;
		_id--;
	}

	bool StackAllocator::IsEmpty() const
	{
		return _current == 0;
	}

	size_t StackAllocator::GetDepth() const
	{
		if (_next)
			return _next->GetDepth() + 1;
		return 0;
	}

	bool StackAllocator::IsOnTop(const AllocationID& allocation) const
	{
		if (_id - 1 == allocation.index && allocation.src == _data)
			return true;
		return _next ? _next->IsOnTop(allocation) : false;
	}

	void StackAllocator::Clear()
	{
		_current = 0;
		_id = 0;
		if(_next)
			_next->Clear();
	}

	size_t StackAllocator::ToChunkSize(const size_t size)
	{
		return size / sizeof(size_t) + (size % sizeof(size_t) > 0);
	}

	void StackAllocator::MFree(const AllocationID& allocation)
	{
		if (_data != allocation.src)
			return _next->MFree(allocation);
		assert(_id == allocation.index + 1);

		MFreeUnsafe(allocation.src);
	}
}
