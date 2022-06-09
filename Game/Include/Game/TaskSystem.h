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
	class TaskSystem : public ISystemChainable
	{
	public:
		[[nodiscard]] Task& operator[](size_t index);

		virtual Task& Add(const Task& task = {});
		virtual void RemoveAt(size_t index);
		void SetCount(size_t count);

		// Increase the size that the system will be allocated with.
		// Call this before actually allocating it.
		void IncreaseRequestedLength(size_t size);
		[[nodiscard]] size_t GetRequestedLength() const;

		[[nodiscard]] jlb::Vector<Task>& GetInstances();
		[[nodiscard]] Task* GetData() const;
		[[nodiscard]] size_t GetLength() const;
		[[nodiscard]] size_t GetCount() const;

		[[nodiscard]] jlb::Iterator<Task> begin();
		[[nodiscard]] jlb::Iterator<Task> end();

	protected:
		void Allocate(const EngineOutData& outData, SystemChain& chain) override;
		void Free(const EngineOutData& outData, SystemChain& chain) override;

	private:
		jlb::Vector<Task> _tasks{};
		size_t _requestedLength = 0;
	};

	template <typename Task>
	Task& TaskSystem<Task>::operator[](const size_t index)
	{
		return _tasks[index];
	}

	template <typename Task>
	Task& TaskSystem<Task>::Add(const Task& task)
	{
		return _tasks.Add(task);
	}

	template <typename Task>
	void TaskSystem<Task>::RemoveAt(const size_t index)
	{
		_tasks.RemoveAt(index);
	}

	template <typename Task>
	void TaskSystem<Task>::SetCount(const size_t count)
	{
		_tasks.SetCount(count);
	}

	template <typename Task>
	void TaskSystem<Task>::IncreaseRequestedLength(const size_t size)
	{
		assert(!_tasks.GetData());
		_requestedLength += size;
	}

	template <typename Task>
	size_t TaskSystem<Task>::GetRequestedLength() const
	{
		return _requestedLength;
	}

	template <typename Task>
	jlb::Iterator<Task> TaskSystem<Task>::begin()
	{
		return _tasks.begin();
	}

	template <typename Task>
	jlb::Iterator<Task> TaskSystem<Task>::end()
	{
		return _tasks.end();
	}

	template <typename Task>
	jlb::Vector<Task>& TaskSystem<Task>::GetInstances()
	{
		return _tasks;
	}

	template <typename Task>
	Task* TaskSystem<Task>::GetData() const
	{
		return _tasks.GetData();
	}

	template <typename Task>
	size_t TaskSystem<Task>::GetLength() const
	{
		return _tasks.GetLength();
	}

	template <typename Task>
	size_t TaskSystem<Task>::GetCount() const
	{
		return _tasks.GetCount();
	}

	template <typename Task>
	void TaskSystem<Task>::Allocate(const EngineOutData& outData, SystemChain& chain)
	{
		_tasks.Allocate(*outData.allocator, _requestedLength);
	}

	template <typename Task>
	void TaskSystem<Task>::Free(const EngineOutData& outData, SystemChain& chain)
	{
		_tasks.Free(*outData.allocator);
	}
}
