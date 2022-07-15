#pragma once
#include "Components/MovementComponent.h"
#include "VkEngine/Systems/GameSystem.h"
#include "VkEngine/Systems/TaskSystemWithOutput.h"

namespace game
{
	struct MovementTask final
	{
		MovementComponent component{};
		size_t duration = 1;
	};

	struct MovementTaskOutput final
	{
		// Remaining active turns.
		size_t remaining = 0;
		// Current linearly interpolated position.
		glm::vec2 position{};
		// Current linearly interpolated rotation.
		float rotation = 0;
		// Current non-linearly interpolated scale multiplier for the bobbing effect.
		float scaleMultiplier = 0;
	};

	// Handles the movement of characters. Also handles the corresponding animation (by scaling) and rotations.
	class MovementSystem final : public vke::TaskSystemWithOutput<MovementTask, MovementTaskOutput>
	{
	public:
		// Object scaling during movement.
		float bobbingScaling = 0.1f;
		// Percent (0-1) of the turn spend on rotation.
		float rotationDuration = .25f;

		// Updates the component with the corresponding task output data.
		static void UpdateComponent(MovementComponent& component, const MovementTaskOutput& output);

	private:
		[[nodiscard]] size_t DefineMinimalUsage(const vke::EngineData& info) override;
		[[nodiscard]] bool ValidateOnTryAdd(const MovementTask& task) override;

		void OnUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::Vector<MovementTask>& tasks, jlb::Vector<MovementTaskOutput>& taskOutputs) override;
	};
}
