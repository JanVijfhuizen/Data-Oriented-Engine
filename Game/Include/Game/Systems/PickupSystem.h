#pragma once
#include "Entities/Entity.h"
#include "VkEngine/Systems/TaskSystem.h"

namespace game
{
	struct PickupTask final
	{
		EntityId instance{};
		EntityId pickup{};
	};

	class PickupSystem : public vke::TaskSystem<PickupTask>
	{
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, 
			const jlb::NestedVector<PickupTask>& tasks) override;
	};
}

