
#include "LinearAllocator.h"
#include <malloc.h>
#include <cassert>
#include "JlbMath.h"

namespace jlb
{
	void LinearAllocator::Allocate(const size_t size)
	{
		assert(!_memory);
		_size = ToChunkSize(size);
		// Allocate N size_t chunks.
		_memory = reinterpret_cast<size_t*>(malloc(_size * sizeof(size_t)));
	}

	void LinearAllocator::Free()
	{
		free(_memory);
		_memory = nullptr;
	}

	void* LinearAllocator::Malloc(size_t size)
	{
		// Assert if there still is enough free space.
		size = ToChunkSize(size);
		assert(size + _current + 1 < _size);

		// Get pointer to the free memory that will be used for this allocation.
		void* current = &_memory[_current];

		// Move N steps forward and store the size of this allocation in the furthest chunk.
		_current += size;
		_memory[_current] = size;
		// Increment by one due to the extra size metadata.
		++_current;

		_totalRequestedSpace = Math::Max(_totalRequestedSpace, _current);
		return current;
	}

	void LinearAllocator::Pop()
	{
		// Assert if there is anything to free.
		assert(_current > 0);
		// Move N places back, based on the amount of memory allocated during the last Malloc.
		_current -= _memory[_current - 1] + 1;
	}

	size_t LinearAllocator::GetAvailableMemorySpace() const
	{
		return (_size - _current - 1) * sizeof(size_t);
	}

	bool LinearAllocator::IsEmpty() const
	{
		return _current == 0;
	}

	size_t LinearAllocator::GetTotalRequestedSpace() const
	{
		return (_totalRequestedSpace + 1) * sizeof(size_t);
	}

	size_t LinearAllocator::ToChunkSize(const size_t size)
	{
		// Rounds up to the nearest integer.
		return size / sizeof(size_t) + (size % sizeof(size_t) > 0);
	}
}
