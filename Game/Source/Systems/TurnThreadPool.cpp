#include "pch.h"
#include "Systems/TurnThreadPool.h"
#include "Systems/TurnSystem.h"

namespace game
{
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

	bool TurnThreadPool::AutoClearOnFrameEnd()
	{
		return false;
	}

	bool TurnThreadPool::ValidateOnTryAdd(const TurnThreadPoolTask& task)
	{
		return _takesTasks ? TaskSystem<TurnThreadPoolTask>::ValidateOnTryAdd(task) : false;
	}
}
