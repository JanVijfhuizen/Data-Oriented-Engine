#include "VkEngine/pch.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

#include <iostream>

#include "JlbMath.h"
//#include <synchapi.h>

namespace vke
{
	void ThreadPoolSystem::ThreadObj::operator()(ThreadPoolSystem* sys) const
	{
		const auto data = sys->GetData();

		while(true)
		{
			//while (sys->_tasksRemaining == 0 && !sys->_stopThreads)
				//Sleep(0);
			if (sys->_stopThreads)
				break;

			// Get task.
			ThreadPoolTask task{};
			{
				std::lock_guard guard{ sys->_getNextTaskMutex };
				task = data[--sys->_tasksRemaining];
				std::cout << "Fetching task" << std::endl;
			}

			task.func(*sys->_threadSharedInfo.info, sys->_threadSharedInfo.systems);
			--sys->_tasksRemaining;
		}
	}

	void ThreadPoolSystem::Allocate(const EngineData& info)
	{
		TaskSystem<ThreadPoolTask>::Allocate(info);

		const auto processorCount = jlb::math::Max<size_t>(1, std::thread::hardware_concurrency() - 1);

		_threads.Allocate(*info.allocator, processorCount);
		for (auto& thread : _threads)
			thread = std::thread(ThreadObj(), this);
	}

	void ThreadPoolSystem::Free(const EngineData& info)
	{
		_threads.Free(*info.allocator);
		TaskSystem<ThreadPoolTask>::Free(info);
	}

	void ThreadPoolSystem::OnPreUpdate(const EngineData& info, 
		const jlb::Systems<EngineData> systems,
		const jlb::Vector<ThreadPoolTask>& tasks)
	{
		TaskSystem<ThreadPoolTask>::OnPreUpdate(info, systems, tasks);

		// Continue the threads.
		_threadSharedInfo.info = &info;
		_threadSharedInfo.systems = systems;
		_tasksRemaining = GetCount();
	}

	void ThreadPoolSystem::OnUpdate(const EngineData& info, 
		const jlb::Systems<EngineData> systems,
		const jlb::Vector<ThreadPoolTask>& tasks)
	{
		TaskSystem<ThreadPoolTask>::OnUpdate(info, systems, tasks);

		// Wait for the threads to finish.
		//while (_tasksRemaining > 0)
			//Sleep(0);
	}

	void ThreadPoolSystem::Exit(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		for (auto& thread : _threads)
			thread.join();
		TaskSystem<ThreadPoolTask>::Exit(info, systems);
	}

	size_t ThreadPoolSystem::DefineMinimalUsage(const EngineData& info)
	{
		return 8;
	}
}
