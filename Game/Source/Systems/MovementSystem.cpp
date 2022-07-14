﻿#include "pch.h"
#include "Systems/MovementSystem.h"
#include "JlbMath.h"
#include "Systems/TurnSystem.h"

namespace game
{
	void MovementSystem::UpdateEntity(MovementComponent& component, const MovementTaskOutput& output, glm::vec2& outPosition)
	{
		component.remaining = output.remaining;
		outPosition = output.position;
	}

	void MovementSystem::OnUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
		const jlb::Vector<MovementTask>& tasks, jlb::Vector<MovementTaskOutput>& taskOutputs)
	{
		const auto turnSys = systems.GetSystem<TurnSystem>();

		const bool isTickEvent = turnSys->GetIfTickEvent();
		const float tickLerp = turnSys->GetTickLerp();
		
		for (const auto& task : tasks)
		{
			const auto& component = task.component;
			assert(component.remaining > 0);
			MovementTaskOutput output{};
			output.remaining = component.remaining - isTickEvent;

			const auto durationF = static_cast<float>(task.duration);
			// 
			const float pct = 1.f / durationF * tickLerp + 1.f - static_cast<float>(output.remaining) / durationF;
			output.position = jlb::math::LerpPct(component.from, component.to, pct);

			// Perfectly set the position once the tick event has been reached if the remaining turns is zero.
			output.position = isTickEvent && output.remaining == 0 ? component.to : output.position;

			taskOutputs.Add(output);
		}
	}

	size_t MovementSystem::DefineMinimalUsage(const vke::EngineData& info)
	{
		return 64;
	}
}
