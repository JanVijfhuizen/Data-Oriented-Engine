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
		size_t remaining = 0;
		glm::vec2 position{};
		float rotation = 0;
		float scaleMultiplier = 0;
	};

	class MovementSystem final : public vke::TaskSystemWithOutput<MovementTask, MovementTaskOutput>
	{
	public:
		float bobbingScaling = 0.1f;
		float rotationDuration = .25f;

		static void UpdateComponent(MovementComponent& component, const MovementTaskOutput& output);

		void OnUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::Vector<MovementTask>& tasks, jlb::Vector<MovementTaskOutput>& taskOutputs) override;

	private:
		[[nodiscard]] size_t DefineMinimalUsage(const vke::EngineData& info) override;
		[[nodiscard]] bool ValidateOnTryAdd(const MovementTask& task) override;
	};
}
