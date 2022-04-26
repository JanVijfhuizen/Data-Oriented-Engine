#pragma once
#include "Vector.h"

namespace jlb
{
	/// <summary>
	/// Entity Component System that iterates over and executes tasks.
	/// </summary>
	/// <typeparam name="Task">Package of all the data needed to execute a task.</typeparam>
	template <typename Task>
	class TaskSystem : public Vector<Task>
	{
	public:
		// Increase the size that the system will be allocated with.
		// Call this before actually allocating it.
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
