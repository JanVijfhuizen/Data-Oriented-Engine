#include "pch.h"
#include "Systems/PickupSystem.h"
#include "Curve.h"
#include "JlbMath.h"
#include "Systems/EntitySystem.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game
{
	void PickupSystem::OnPreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<PickupComponent>& tasks)
	{
		TaskSystemWithOutput<PickupComponent, PickupComponent>::OnPreUpdate(info, systems, tasks);

		const auto turnSys = systems.GetSystem<TurnSystem>();

		vke::ThreadPoolTask threadTask{};
		threadTask.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
		{
			const auto self = static_cast<PickupSystem*>(userPtr);
			const auto turnSys = systems.GetSystem<TurnSystem>();

			const bool isEndTickEvent = turnSys->GetIfEndTickEvent();

			const auto& tasks = self->GetTasks();
			auto curveOvershoot = jlb::CreateCurveOvershooting();
			auto& tasksOutput = self->GetOutputEditable();
			auto& dumpAllocator = *info.dumpAllocator;

			const float lerp = turnSys->GetTickLerp();
			const float eval = jlb::DoubleCurveEvaluate(lerp, curveOvershoot, curveOvershoot);

			for (auto& task : tasks)
			{
				if (task.active)
				{
					const auto pos = jlb::math::LerpPct(task._instancePosition, task._pickupPosition, eval);
					task.outHandPosition = pos;
					task.active = !isEndTickEvent;
				}
				
				tasksOutput.Add(dumpAllocator, task);
			}
		};

		threadTask.userPtr = this;

		auto& tasksOutput = GetOutputEditable();
		auto& dumpAllocator = *info.dumpAllocator;
		tasksOutput.PreAllocateNested(dumpAllocator, GetCount());

		const auto threadSys = systems.GetSystem<vke::ThreadPoolSystem>();
		const auto result = threadSys->TryAdd(info, threadTask);
		assert(result != SIZE_MAX);

		const auto entitySys = systems.GetSystem<EntitySystem>();

		if(turnSys->GetIfBeginTickEvent())
			for (auto& task : tasks)
			{
				task.active = entitySys->Contains(task.inInstanceId) && entitySys->Contains(task.inPickupId);
				if (!task.active)
					continue;

				auto& instance = entitySys->operator[](task.inInstanceId.index);
				auto& pickup = entitySys->operator[](task.inPickupId.index);
				instance.character.inventory.Insert(pickup.pickup.cardId);
				pickup.markedForDelete = true;

				task._instancePosition = instance.position;
				task._pickupPosition = pickup.position;
			}
	}
}
