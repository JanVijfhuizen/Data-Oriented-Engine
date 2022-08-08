#include "pch.h"
#include "Systems/InteractSystem.h"
#include "Systems/EntitySystem.h"
#include "Systems/TurnSystem.h"
#include "Systems/TurnThreadPoolSystem.h"

namespace game
{
	size_t InteractSystem::DefineCapacity(const vke::EngineData& info)
	{
		return ENTITY_CAPACITY;
	}

	void InteractSystem::OnPreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<InteractionTask>& tasks)
	{
		TaskSystem<InteractionTask>::OnPreUpdate(info, systems, tasks);
		const auto turnSys = systems.GetSystem<TurnSystem>();
		if (turnSys->GetIfTickEvent())
		{
			TurnThreadPoolTask task{};
			task.userPtr = this;
			task.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
			{
				const auto sys = reinterpret_cast<InteractSystem*>(userPtr);

				auto& entities = systems.GetSystem<EntitySystem>()->GetTasks();
				auto& tasks = sys->GetTasks();

				for (const auto& task : tasks)
				{
					auto& target = entities[task.target];
					auto& src = entities[task.src];
					task.interaction(target, src, task.userPtr);
				}

				sys->ClearTasks();
			};

			const auto turnThreadSys = systems.GetSystem<TurnThreadPoolSystem>();
			const auto result = turnThreadSys->TryAdd(info, task);
			assert(result != SIZE_MAX);
		}
	}

	bool InteractSystem::AutoClearOnFrameEnd()
	{
		return false;
	}
}
