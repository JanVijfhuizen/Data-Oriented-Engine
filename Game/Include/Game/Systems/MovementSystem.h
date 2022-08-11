#pragma once
#include "Components/MovementComponent.h"
#include "VkEngine/Systems/GameSystem.h"
#include "VkEngine/Systems/TaskSystemWithOutput.h"

namespace game
{
	typedef MovementComponent MovementTask;

	// Handles the movement of characters. Also handles the corresponding animation (by scaling) and rotations.
	class MovementSystem final : public vke::TaskSystemWithOutput<MovementTask, MovementTask>
	{
	public:
		// Object scaling during movement.
		float bobbingScaling = 0.1f;
		// Percent (0-1) of the turn spend on rotation.
		float rotationDuration = .25f;
		
	private:
		[[nodiscard]] size_t DefineCapacity(const vke::EngineData& info) override;
		[[nodiscard]] bool ValidateOnTryAdd(const MovementTask& task) override;

		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::NestedVector<MovementTask>& tasks) override;
	};
}
