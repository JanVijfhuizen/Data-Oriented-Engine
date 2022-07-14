#pragma once
#include "Components/MovementComponent.h"
#include "VkEngine/Systems/GameSystem.h"
#include "VkEngine/Systems/TaskSystemWithOutput.h"

namespace game
{
	struct MovementTask final
	{
		MovementComponent component{};
		size_t speed = 1;
	};

	struct MovementTaskOutput final
	{
		size_t remaining = 0;
		glm::vec2 position{};
	};

	class MovementSystem final : public vke::TaskSystemWithOutput<MovementTask, MovementTaskOutput>
	{
	public:
		static void UpdateEntity(MovementComponent& component, const MovementTaskOutput& output, glm::vec2& outPosition);

		void OnUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::Vector<MovementTask>& tasks, jlb::Vector<MovementTaskOutput>& taskOutputs) override;
	};
}
