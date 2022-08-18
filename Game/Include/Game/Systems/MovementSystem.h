#pragma once
#include "ThreadedTaskSystemWithOutput.h"
#include "Components/MovementComponent.h"

namespace game
{
	typedef MovementComponent MovementTask;

	// Handles the movement of characters. Also handles the corresponding animation (by scaling) and rotations.
	class MovementSystem final : public ThreadedTaskSystemWithOutput<MovementSystem, MovementTask, MovementTask>
	{
	public:
		// How many times bobbing happens during a tick.
		float bobbingAmount = 2;
		// Object scaling during movement.
		float bobbingScaling = 0.1f;
		// Percent (0-1) of the turn spend on rotation.
		float rotationDuration = 1;
		
	private:
		[[nodiscard]] size_t DefineCapacity(const vke::EngineData& info) override;
		[[nodiscard]] bool ValidateOnTryAdd(const MovementTask& task) override;
		[[nodiscard]] threadableFunc DefineThreadable() override;
	};
}
