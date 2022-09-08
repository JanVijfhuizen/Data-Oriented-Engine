#pragma once
#include "ParallelJobSystemWithOutput.h"
#include "Components/MovementComponent.h"

namespace game
{
	typedef MovementComponent MovementJob;

	// Handles the movement of characters. Also handles the corresponding animation (by scaling) and rotations.
	class MovementSystem final : public ParallelJobSystemWithOutput<MovementSystem, MovementJob, MovementJob>
	{
	public:
		// How many times bobbing happens during a tick.
		float bobbingAmount = 2;
		// Object scaling during movement.
		float bobbingScaling = 0.1f;
		// Percent (0-1) of the turn spend on rotation.
		float rotationDuration = 1;
		// How much a character moves when the path is obstructed before moving back.
		float obstructedMovementDistance = .2f;
		
	private:
		[[nodiscard]] size_t DefineCapacity(const vke::EngineData& info) override;
		[[nodiscard]] bool ValidateOnTryAdd(const MovementJob& task) override;
		[[nodiscard]] threadableFunc DefineThreadable() override;
	};
}
