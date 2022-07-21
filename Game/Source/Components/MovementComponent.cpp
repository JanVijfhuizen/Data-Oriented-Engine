#include "pch.h"
#include "Components/MovementComponent.h"
#include "Systems/MovementSystem.h"

namespace game
{
	void MovementComponent::Build()
	{
		systemDefined.remaining = userDefined.duration;
	}

	void MovementComponent::Update(const MovementTaskOutput& output)
	{
		systemDefined.remaining = output.remaining;
		systemDefined.scaleMultiplier = output.scaleMultiplier;
	}
}
