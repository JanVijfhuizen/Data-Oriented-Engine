#include "pch.h"
#include "Systems/MovementSystem.h"

#include <iostream>

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
			const float pct = tickLerp / durationF + 1.f -
				static_cast<float>(component.remaining) / durationF;
			output.position = jlb::math::LerpPct(component.from, component.to, pct);

			// TEMP
			if (isTickEvent)
				std::cout << std::endl;
			std::cout << pct << std::endl;

			taskOutputs.Add(output);
		}
	}

	size_t MovementSystem::DefineMinimalUsage(const vke::EngineData& info)
	{
		return 64;
	}
}
