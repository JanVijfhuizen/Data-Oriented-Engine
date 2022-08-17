#include "pch.h"
#include "Systems/MovementSystem.h"
#include "Curve.h"
#include "JlbMath.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game
{
	size_t MovementSystem::DefineCapacity(const vke::EngineData& info)
	{
		return ENTITY_CAPACITY;
	}

	bool MovementSystem::ValidateOnTryAdd(const MovementTask& task)
	{
		return !task.active ? false : TaskSystemWithOutput<MovementTask, MovementTask>::ValidateOnTryAdd(task);
	}

	void MovementThreadableTask(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<MovementComponent>& tasks, jlb::NestedVector<MovementComponent>& output, 
		jlb::StackAllocator& dumpAllocator, MovementSystem* selfPtr)
	{
		const auto turnSys = systems.GetSystem<TurnSystem>();

		const bool isEndTickEvent = turnSys->GetIfEndTickEvent();
		const float tickLerp = turnSys->GetTickLerp();

		auto curveOvershoot = jlb::CreateCurveOvershooting();

		for (auto& task : tasks)
		{
			task.outRemaining -= isEndTickEvent;

			const auto durationF = static_cast<float>(task.inDuration);
			// Smoothly move between grid positions.
			const float pct = 1.f / durationF * tickLerp + 1.f - static_cast<float>(task.outRemaining) / durationF;
			task.outPosition = jlb::math::LerpPct(task.inFrom, task.inTo, pct);

			// Bobbing.
			const float bobbingPct = fmodf(pct * selfPtr->bobbingAmount, 1);
			const float eval = jlb::DoubleCurveEvaluate(bobbingPct, curveOvershoot, curveOvershoot);
			task.outScaleMultiplier = 1.f + eval * selfPtr->bobbingScaling;

			// Movement rotation.
			const float toAngle = jlb::math::GetAngle(task.inFrom, task.inTo);
			const float pctRotation = jlb::math::Min<float>(pct / selfPtr->rotationDuration, 1);

			task.outRotation = jlb::math::SmoothAngle(task.outRotation, toAngle, curveOvershoot.Evaluate(pctRotation));

			const bool finished = task.outRemaining == 0;
			task.outPosition = finished ? task.inTo : task.outPosition;
			task.outRotation = finished ? toAngle : task.outRotation;

			task.active = !finished;
			output.Add(dumpAllocator, task);
		}
	}

	ThreadedTaskSystemWithOutput<MovementSystem, MovementComponent, MovementComponent>::threadableFunc
		MovementSystem::DefineThreadable()
	{
		return MovementThreadableTask;
	}
}
