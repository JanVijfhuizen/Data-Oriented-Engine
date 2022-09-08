#pragma once
#include "Components/PickupComponent.h"
#include "VkEngine/Systems/JobSystemWithOutput.h"

namespace game
{
	class PickupSystem : public vke::JobSystemWithOutput<PickupComponent, PickupComponent>
	{
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, 
			const jlb::NestedVector<PickupComponent>& jobs) override;
	};
}

