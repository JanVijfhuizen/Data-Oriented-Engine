#include "pch.h"
#include "Systems/InteractSystem.h"
#include "Systems/EntitySystem.h"

namespace game
{
	size_t InteractSystem::DefineCapacity(const vke::EngineData& info)
	{
		return ENTITY_CAPACITY;
	}

	void InteractSystem::OnUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<InteractionTask>& tasks)
	{
		TaskSystem<InteractionTask>::OnUpdate(info, systems, tasks);

		auto& entities = systems.GetSystem<EntitySystem>()->GetTasks();

		for (auto& task : tasks)
		{
			auto& target = entities[task.target];
			auto& src = entities[task.src];
			task.interaction(target, src, task.data);
		}
	}
}
