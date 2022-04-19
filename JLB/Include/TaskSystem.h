#pragma once
#include "Vector.h"

namespace jlb
{
	template <typename Task>
	class TaskSystem : public Vector<Task>
	{
	public:
		void IncreaseRequestedLength(size_t size);
		virtual void Allocate(LinearAllocator& allocator);

	private:
		using Vector<Task>::Allocate;
		using Vector<Task>::AllocateAndCopy;

		size_t _requestedLength = 0;
	};

	template <typename Task>
	void TaskSystem<Task>::IncreaseRequestedLength(const size_t size)
	{
		_requestedLength += size;
	}

	template <typename Task>
	void TaskSystem<Task>::Allocate(LinearAllocator& allocator)
	{
		Vector<Task>::Allocate(allocator, _requestedLength);
	}
}
