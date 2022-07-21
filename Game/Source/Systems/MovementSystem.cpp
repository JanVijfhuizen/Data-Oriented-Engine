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
		TaskSystemWithOutput<MovementTask, MovementTaskOutput>::OnPreUpdate(info, systems, tasks);

		vke::ThreadPoolTask threadTask{};
		threadTask.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
		{
			const auto self = static_cast<MovementSystem*>(userPtr);
			const auto turnSys = systems.GetSystem<TurnSystem>();

			const bool isTickEvent = turnSys->GetIfTickEvent();
			const float tickLerp = turnSys->GetTickLerp();

			auto curveOvershoot = jlb::CreateCurveOvershooting();

			const auto& tasks = self->GetTasks();
			auto& tasksOutput = self->GetOutputEditable();
			auto& dumpAllocator = *info.dumpAllocator;

			for (const auto& task : tasks)
			{
				const auto& settings = task.settings;
				const auto& userDefined = task.userDefined;
				const auto& systemDefined = task.systemDefined;
				
				MovementTaskOutput output{};
				output.remaining = systemDefined.remaining - isTickEvent;

				const auto durationF = static_cast<float>(userDefined.duration);
				// Smoothly move between grid positions.
				const float pct = 1.f / durationF * tickLerp + 1.f - static_cast<float>(output.remaining) / durationF;
				output.position = jlb::math::LerpPct(userDefined.from, userDefined.to, pct);

				const bool finished = isTickEvent && output.remaining == 0;

				// Perfectly set the position once the tick event has been reached if the remaining turns is zero.
				output.position = finished ? userDefined.to : output.position;

				// Bobbing.
				const float bobbingPct = fmodf(pct * settings.bobbingAmount, 1);
				const float eval = jlb::DoubleCurveEvaluate(bobbingPct, curveOvershoot, curveOvershoot);
				output.scaleMultiplier = 1.f + eval * self->bobbingScaling;

				// Movement rotation.
				const float toAngle = jlb::math::GetAngle(userDefined.from, userDefined.to);
				const float pctRotation = jlb::math::Min<float>(pct / self->rotationDuration, 1);

				output.rotation = jlb::math::SmoothAngle(userDefined.rotation, toAngle, curveOvershoot.Evaluate(pctRotation));
				output.rotation = finished ? toAngle : output.rotation;
				tasksOutput.Add(dumpAllocator, output);
			}
		};
		threadTask.userPtr = this;

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
		return task.systemDefined.remaining == 0 ? false : TaskSystemWithOutput<MovementTask, MovementTaskOutput>::ValidateOnTryAdd(task);
	}
}
