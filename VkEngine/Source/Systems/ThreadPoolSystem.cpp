#include "VkEngine/pch.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"
#include "JlbMath.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace vke
{
	void ThreadPoolSystem::ThreadObj::operator()(ThreadPoolSystem* sys) const
	{
		auto& tasks = sys->GetTasks();

		while(!sys->_stopThreads)
		{
			if(sys->_tasksRemaining == 0)
			{
				Sleep(0);
				continue;
			}

			// Get task.
			sys->_getNextTaskMutex.lock();
			// When another thread has stolen the task already.
			if(sys->_tasksRemaining == 0)
			{
				sys->_getNextTaskMutex.unlock();
				continue;
			}
			const auto task = tasks[--sys->_tasksRemaining];
			sys->_getNextTaskMutex.unlock();

			task.func(*sys->_threadSharedInfo.info, sys->_threadSharedInfo.systems, task.userPtr);
			--sys->_tasksUnfinished;
		}
	}

	void ThreadPoolSystem::Allocate(const EngineData& info)
	{
		TaskSystem<ThreadPoolTask>::Allocate(info);

		_threadCount = GetThreadCount();
		_threads = info.allocator->New<std::thread>(_threadCount, ThreadObj(), this);
	}

	void ThreadPoolSystem::Free(const EngineData& info)
	{
		info.allocator->MFree(_threads.id);
		TaskSystem<ThreadPoolTask>::Free(info);
	}

	void ThreadPoolSystem::OnUpdate(const EngineData& info, 
		const jlb::Systems<EngineData> systems,
		const jlb::NestedVector<ThreadPoolTask>& tasks)
	{
		TaskSystem<ThreadPoolTask>::OnPreUpdate(info, systems, tasks);

		// Continue the threads.
		_threadSharedInfo.info = &info;
		_threadSharedInfo.systems = systems;
		_tasksRemaining = _tasksUnfinished = tasks.GetCount();
	}

	void ThreadPoolSystem::OnPostUpdate(const EngineData& info, 
		const jlb::Systems<EngineData> systems,
		const jlb::NestedVector<ThreadPoolTask>& tasks)
	{
		TaskSystem<ThreadPoolTask>::OnUpdate(info, systems, tasks);
		
		// Wait for the threads to finish.
		while (_tasksUnfinished > 0)
			Sleep(0);
	}

	void ThreadPoolSystem::Exit(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		_stopThreads = true;
		for (int i = 0; i < _threadCount; ++i)
			_threads.ptr[i].join();
		TaskSystem<ThreadPoolTask>::Exit(info, systems);
	}

	size_t ThreadPoolSystem::DefineCapacity(const EngineData& info)
	{
		return THREAD_POOL_SYSTEM_CAPACITY;
	}

	size_t ThreadPoolSystem::DefineNestedCapacity(const EngineData& info)
	{
		return THREAD_POOL_SYSTEM_NESTED_CAPACITY;
	}

	size_t ThreadPoolSystem::GetThreadCount()
	{
		return jlb::math::Max<size_t>(1, std::thread::hardware_concurrency() - 1 - THREAD_POOL_SYSTEM_UNUSED_THREAD_COUNT);
	}

	size_t ThreadPoolSystem::GetFreeThreadSlots() const
	{
		return _threadCount < _tasksUnfinished ? 0 : _threadCount - _tasksUnfinished;
	}
}
