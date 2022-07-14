#include "pch.h"
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
			MovementTaskOutput output{};
			output.remaining = component.remaining - isTickEvent;

			const float pct = tickLerp / static_cast<float>(task.speed) + 1.f - 
				static_cast<float>(component.remaining) / static_cast<float>(task.speed);
			output.position = jlb::math::LerpPct(component.from, component.to, pct);

			taskOutputs.Add(output);
		}
	}
}
