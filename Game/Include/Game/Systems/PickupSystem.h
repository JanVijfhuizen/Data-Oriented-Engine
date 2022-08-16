#pragma once
#include "Entities/Entity.h"
#include "VkEngine/Systems/TaskSystemWithOutput.h"

namespace game
{
	struct PickupTask final
	{
		friend class PickupSystem;

		EntityId instance{};
		EntityId pickup{};

	private:
		glm::vec2 _position{};
		glm::vec2 _pickupPosition{};
	};

	struct PickupTaskOutput final
	{
		glm::vec2 lHandPosition{};
		glm::vec2 rHandPosition{};
	};

	class PickupSystem : public vke::TaskSystemWithOutput<PickupTask, PickupTaskOutput>
	{
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, 
			const jlb::NestedVector<PickupTask>& tasks) override;
	};
}

