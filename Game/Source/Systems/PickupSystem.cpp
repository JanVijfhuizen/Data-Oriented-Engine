#include "pch.h"
#include "Systems/PickupSystem.h"
#include "Systems/EntitySystem.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game
{
	void PickupSystem::OnPreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<PickupTask>& tasks)
	{
		TaskSystem<PickupTask>::OnPreUpdate(info, systems, tasks);

		const auto turnSys = systems.GetSystem<TurnSystem>();

		vke::ThreadPoolTask threadTask{};
		threadTask.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
		{
			// TODO fancy animations.
		};

		threadTask.userPtr = this;
		
		const auto threadSys = systems.GetSystem<vke::ThreadPoolSystem>();
		const auto result = threadSys->TryAdd(info, threadTask);
		assert(result != SIZE_MAX);

		const auto entitySys = systems.GetSystem<EntitySystem>();

		if(turnSys->GetIfBeginTickEvent())
			for (auto& task : tasks)
			{
				if (!entitySys->Contains(task.instance) || !entitySys->Contains(task.pickup))
					continue;

				auto& instance = entitySys->operator[](task.instance.index);
				auto& pickup = entitySys->operator[](task.pickup.index);
				instance.character.inventory.Insert(pickup.pickup.cardId);
				pickup.markedForDelete = true;
			}
	}
}
