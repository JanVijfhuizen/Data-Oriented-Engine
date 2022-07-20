#include "pch.h"
#include "Systems/CollisionSystem.h"
#include <Systems/TurnSystem.h>

namespace game
{
	void CollisionSystem::OnPreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<CollisionTask>& tasks)
	{
		TaskSystem<CollisionTask>::OnPreUpdate(info, systems, tasks);
		const auto turnSys = systems.GetSystem<TurnSystem>();

		// If the previous frame was for adding tasks.
		if (_mayAddTasks)
		{
			// Compile into collision distance tree.
		}

		// If tick event, clear tasks.
		_mayAddTasks = turnSys->GetIfTickEvent();
		if (_mayAddTasks)
			ClearTasks();
	}

	bool CollisionSystem::ValidateOnTryAdd(const CollisionTask& task)
	{
		return _mayAddTasks ? TaskSystem<CollisionTask>::ValidateOnTryAdd(task) : false;
	}

	bool CollisionSystem::AutoClearOnFrameEnd()
	{
		return false;
	}
}
