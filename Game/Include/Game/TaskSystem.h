#pragma once
#include "Vector.h"
#include "SystemChain.h"

namespace game
{
	/// <summary>
	/// Entity Component System that iterates over and executes tasks.
	/// </summary>
	/// <typeparam name="Task">Package of all the data needed to execute a task.</typeparam>
	template <typename Task>
	class TaskSystem : public jlb::Vector<Task>, public ISystemChainable
	{
	public:
		// Increase the size that the system will be allocated with.
		// Call this before actually allocating it.
		void IncreaseRequestedLength(size_t size);

	protected:
		void Allocate(const EngineOutData& outData, SystemChain& chain) override;
		void Free(const EngineOutData& outData, SystemChain& chain) override;

	private:
		using jlb::Vector<Task>::Allocate;
		using jlb::Vector<Task>::AllocateAndCopy;
		using jlb::Vector<Task>::Free;

		size_t _requestedLength = 0;
	};

	template <typename Task>
	void TaskSystem<Task>::IncreaseRequestedLength(const size_t size)
	{
		_requestedLength += size;
	}

	template <typename Task>
	void TaskSystem<Task>::Allocate(const EngineOutData& outData, SystemChain& chain)
	{
		jlb::Vector<Task>::Allocate(outData.allocator, _requestedLength);
	}

	template <typename Task>
	void TaskSystem<Task>::Free(const EngineOutData& outData, SystemChain& chain)
	{
		jlb::Vector<Task>::Free(outData.allocator);
	}
}
