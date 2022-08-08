#pragma once
#include "Archetype.h"
#include "Entities/Pickup.h"

namespace game
{
	class PickupArchetype final : public Archetype<Pickup>
	{
	public:
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			jlb::ArrayView<Pickup> entities) override;
	};
}
