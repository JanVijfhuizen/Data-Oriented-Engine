#pragma once
#include "Components/PickupComponent.h"
#include "VkEngine/Systems/TaskSystemWithOutput.h"

namespace game
{
	class PickupSystem : public vke::TaskSystemWithOutput<PickupComponent, PickupComponent>
	{
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, 
			const jlb::NestedVector<PickupComponent>& tasks) override;
	};
}

