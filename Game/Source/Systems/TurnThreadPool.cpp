#include "pch.h"
#include "Systems/TurnThreadPool.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game
{
	void TurnThreadPool::ManagingThread::operator()(TurnThreadPool* sys, vke::ThreadPoolSystem* threadPoolSys) const
	{
		auto& tasks = sys->GetTasks();

		while (!sys->_stopThreads)
		{
			if (sys->_tasksRemaining == 0)
			{
				Sleep(0);
				continue;
			}

			const size_t freeSlots = threadPoolSys->GetFreeThreadSlots();
			if(freeSlots == 0)
			{
				Sleep(0);
				continue;
			}
		}
	}

	void TurnThreadPool::Allocate(const vke::EngineData& info)
	{
		TaskSystem<TurnThreadPoolTask>::Allocate(info);
		_threadCount = vke::ThreadPoolSystem::GetThreadCount();
		_managingThread = info.allocator->New<std::thread>(1, ManagingThread(), this);
		
	}

	void TurnThreadPool::Free(const vke::EngineData& info)
	{
		info.allocator->MFree(_managingThread.id);
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
		ClearTasks();
	}

	void TurnThreadPool::OnPostUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<TurnThreadPoolTask>& tasks)
	{
		TaskSystem<TurnThreadPoolTask>::OnPostUpdate(info, systems, tasks);

		if (_takesTasks) 
		{
			_takesTasks = false;
			_tasksRemaining = tasks.GetCount();
		}
	}

	void TurnThreadPool::Exit(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		_stopThreads = true;
		_managingThread.ptr->join();
		//for (int i = 0; i < _threadCount; ++i)
			//_threads.ptr[i].join();
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
