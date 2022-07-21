#include "pch.h"
#include "Systems/TurnThreadPoolSystem.h"
#include "Systems/TurnSystem.h"

namespace game
{
	void TurnThreadPoolSystem::ThreadObj::operator()(TurnThreadPoolSystem* sys) const
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
			const auto task = tasks[--sys->_tasksRemaining];
			task.func(*sys->_threadSharedInfo.info, sys->_threadSharedInfo.systems, task.userPtr);
			--sys->_tasksUnfinished;
		}
	}

	void TurnThreadPoolSystem::Allocate(const vke::EngineData& info)
	{
		TaskSystem<TurnThreadPoolTask>::Allocate(info);
		_thread = info.allocator->New<std::thread>(1, ThreadObj(), this);
	}

	void TurnThreadPoolSystem::Free(const vke::EngineData& info)
	{
		info.allocator->MFree(_thread.id);
		TaskSystem<TurnThreadPoolTask>::Free(info);
	}

	void TurnThreadPoolSystem::OnPreUpdate(const vke::EngineData& info, 
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

	void TurnThreadPoolSystem::OnPostUpdate(const vke::EngineData& info, 
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

	void TurnThreadPoolSystem::Exit(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		_stopThreads = true;
		_thread.ptr->join();
		TaskSystem<TurnThreadPoolTask>::Exit(info, systems);
	}

	bool TurnThreadPoolSystem::AutoClearOnFrameEnd()
	{
		return false;
	}

	bool TurnThreadPoolSystem::ValidateOnTryAdd(const TurnThreadPoolTask& task)
	{
		return _takesTasks ? TaskSystem<TurnThreadPoolTask>::ValidateOnTryAdd(task) : false;
	}

	size_t TurnThreadPoolSystem::DefineCapacity(const vke::EngineData& info)
	{
		// WIP nested vector gives deallocation issues.
		return 0;// 8;
	}
}
