#include "pch.h"
#include "Systems/MovementSystem.h"
#include "Curve.h"
#include "JlbMath.h"
#include "Systems/TurnSystem.h"

namespace game
{
	void MovementSystem::UpdateComponent(MovementComponent& component, const MovementTaskOutput& output)
	{
		component.remaining = output.remaining;
		component.scaleMultiplier = output.scaleMultiplier;
	}

	void MovementSystem::OnUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
		const jlb::Vector<MovementTask>& tasks, jlb::Vector<MovementTaskOutput>& taskOutputs)
	{
		const auto turnSys = systems.GetSystem<TurnSystem>();

		const bool isTickEvent = turnSys->GetIfTickEvent();
		const float tickLerp = turnSys->GetTickLerp();

		auto curveOvershoot = jlb::CreateCurveOvershooting();
		
		for (const auto& task : tasks)
		{
			const auto& component = task.component;
			assert(component.remaining > 0);
			MovementTaskOutput output{};
			output.remaining = component.remaining - isTickEvent;

			const auto durationF = static_cast<float>(task.duration);
			// Smoothly move between grid positions.
			const float pct = 1.f / durationF * tickLerp + 1.f - static_cast<float>(output.remaining) / durationF;
			output.position = jlb::math::LerpPct(component.from, component.to, pct);

			const bool finished = isTickEvent && output.remaining == 0;

			// Perfectly set the position once the tick event has been reached if the remaining turns is zero.
			output.position = finished ? component.to : output.position;

			// Bobbing.
			const float bobbingPct = fmodf(pct * component.bobbingAmount, 1);
			const float eval = jlb::DoubleCurveEvaluate(bobbingPct, curveOvershoot, curveOvershoot);
			output.scaleMultiplier = 1.f + eval * bobbingScaling;

			// Movement rotation.
			const float toAngle = jlb::math::GetAngle(component.from, component.to);
			const float pctRotation = jlb::math::Min<float>(pct / rotationDuration, 1);

			output.rotation = jlb::math::SmoothAngle(task.component.rotation, toAngle, curveOvershoot.Evaluate(pctRotation));
			output.rotation = finished ? toAngle : output.rotation;
			taskOutputs.Add(output);
		}
	}

	size_t MovementSystem::DefineMinimalUsage(const vke::EngineData& info)
	{
		return 64;
	}
}
