#include "pch.h"
#include "Systems/MovementSystem.h"
#include "Curve.h"
#include "JlbMath.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game
{
	void MovementSystem::OnPreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<MovementTask>& tasks)
	{
		TaskSystemWithOutput<MovementTask, MovementTask>::OnPreUpdate(info, systems, tasks);

		vke::ThreadPoolTask threadTask{};
		threadTask.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
		{
			const auto self = static_cast<MovementSystem*>(userPtr);
			const auto turnSys = systems.GetSystem<TurnSystem>();

			const bool isEndTickEvent = turnSys->GetIfEndTickEvent();
			const float tickLerp = turnSys->GetTickLerp();

			auto curveOvershoot = jlb::CreateCurveOvershooting();

			const auto& tasks = self->GetTasks();
			auto& tasksOutput = self->GetOutputEditable();
			auto& dumpAllocator = *info.dumpAllocator;

			for (auto& task : tasks)
			{
				task.remaining -= isEndTickEvent;

				const auto durationF = static_cast<float>(task.duration);
				// Smoothly move between grid positions.
				const float pct = 1.f / durationF * tickLerp + 1.f - static_cast<float>(task.remaining) / durationF;
				task.position = jlb::math::LerpPct(task.from, task.to, pct);

				// Bobbing.
				const float bobbingPct = fmodf(pct * task.bobbingAmount, 1);
				const float eval = jlb::DoubleCurveEvaluate(bobbingPct, curveOvershoot, curveOvershoot);
				task.scaleMultiplier = 1.f + eval * self->bobbingScaling;

				// Movement rotation.
				const float toAngle = jlb::math::GetAngle(task.from, task.to);
				const float pctRotation = jlb::math::Min<float>(pct / self->rotationDuration, 1);

				const bool finished = task.remaining == 0;
				task.position = finished ? task.to : task.position;
				task.rotation = finished ? toAngle : task.rotation;

				task.rotation = jlb::math::SmoothAngle(task.rotation, toAngle, curveOvershoot.Evaluate(pctRotation));
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
	}

	size_t MovementSystem::DefineCapacity(const vke::EngineData& info)
	{
		return ENTITY_CAPACITY;
	}

	bool MovementSystem::ValidateOnTryAdd(const MovementTask& task)
	{
		return task.remaining == SIZE_MAX ? false : TaskSystemWithOutput<MovementTask, MovementTask>::ValidateOnTryAdd(task);
	}
}
