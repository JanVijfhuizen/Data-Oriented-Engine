#include "pch.h"
#include "Systems/EntitySystem.h"
#include "Systems/TurnSystem.h"

namespace game
{
	size_t EntitySystem::DefineCapacity(const vke::EngineData& info)
	{
		return ENTITY_CAPACITY;
	}

	void EntitySystem::OnPreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<EntityData>& tasks)
	{
		TaskSystem<EntityData>::OnPreUpdate(info, systems, tasks);
		const auto turnSys = systems.GetSystem<TurnSystem>();
		if (turnSys->GetIfBeginTickEvent())
			ClearTasks();
	}

	bool EntitySystem::AutoClearOnFrameEnd()
	{
		return false;
	}
}
