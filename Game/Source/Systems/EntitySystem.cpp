#include "pch.h"
#include "Systems/EntitySystem.h"
#include "Systems/TurnSystem.h"

namespace game
{
	size_t EntitySystem::DefineCapacity(const vke::EngineData& info)
	{
		return ENTITY_CAPACITY;
	}

	void EntitySystem::OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<Entity>& tasks)
	{
		TaskSystem<Entity>::OnPreUpdate(info, systems, tasks);
		const auto turnSys = systems.GetSystem<TurnSystem>();
		if (turnSys->GetIfTickEvent())
			ClearTasks();
	}

	bool EntitySystem::AutoClearOnFrameEnd()
	{
		return false;
	}
}
