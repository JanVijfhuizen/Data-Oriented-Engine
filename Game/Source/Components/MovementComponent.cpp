#include "pch.h"
#include "Components/MovementComponent.h"
#include "Systems/MovementSystem.h"

namespace game
{
	void MovementComponent::Build()
	{
		systemDefined.remaining = userDefined.duration;
	}

	void MovementComponent::Finish()
	{
		systemDefined.remaining = 0;
	}

	void MovementComponent::Update(const MovementTaskOutput& output)
	{
		const bool finished = output.remaining == 0;
		systemDefined.scaleMultiplier = finished ? systemDefined.scaleMultiplier : output.scaleMultiplier;
		systemDefined.remaining = finished ? systemDefined.remaining : output.remaining;
	}
}
