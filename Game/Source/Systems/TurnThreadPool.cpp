#include "pch.h"
#include "Systems/TurnThreadPool.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game
{
	void TurnThreadPool::ThreadObj::operator()(TurnThreadPool* sys) const
	{
		auto& tasks = sys->GetTasks();

		while (!sys->_stopThreads)
		{
			if (sys->_tasksRemaining == 0)
			{
				Sleep(0);
				continue;
			}

			// Get task.
			sys->_getNextTaskMutex.lock();
			// When another thread has stolen the task already.
			if (sys->_tasksRemaining == 0)
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

	void TurnThreadPool::Allocate(const vke::EngineData& info)
	{
		TaskSystem<TurnThreadPoolTask>::Allocate(info);
		_threadCount = vke::ThreadPoolSystem::GetThreadCount();
		_threads = info.allocator->New<std::thread>(_threadCount, ThreadObj(), this);
	}

	void TurnThreadPool::Free(const vke::EngineData& info)
	{
		info.allocator->MFree(_threads.id);
		TaskSystem<TurnThreadPoolTask>::Free(info);
	}

	void TurnThreadPool::OnPreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<TurnThreadPoolTask>& tasks)
	{
		TaskSystem<TurnThreadPoolTask>::OnPreUpdate(info, systems, tasks);

		const auto turnSys = systems.GetSystem<TurnSystem>();
		const bool isTickEvent = turnSys->GetIfTickEvent();
		_takesTasks = isTickEvent;

		if(isTickEvent)
		{
			// Wait for the threads to finish.
			while (_tasksUnfinished > 0)
				Sleep(0);
			ClearTasks();
		}
	}

	void TurnThreadPool::OnPostUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<TurnThreadPoolTask>& tasks)
	{
		TaskSystem<TurnThreadPoolTask>::OnPostUpdate(info, systems, tasks);

		if (_takesTasks) 
		{
			_takesTasks = false;

			// Continue the threads.
			_threadSharedInfo.info = &info;
			_threadSharedInfo.systems = systems;
			_tasksRemaining = _tasksUnfinished = tasks.GetCount();
		}
	}

	void TurnThreadPool::Exit(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		_stopThreads = true;
		for (int i = 0; i < _threadCount; ++i)
			_threads.ptr[i].join();
		TaskSystem<TurnThreadPoolTask>::Exit(info, systems);
	}

	bool TurnThreadPool::AutoClearOnFrameEnd()
	{
		return false;
	}

	bool TurnThreadPool::ValidateOnTryAdd(const TurnThreadPoolTask& task)
	{
		return _takesTasks ? TaskSystem<TurnThreadPoolTask>::ValidateOnTryAdd(task) : false;
	}
}
