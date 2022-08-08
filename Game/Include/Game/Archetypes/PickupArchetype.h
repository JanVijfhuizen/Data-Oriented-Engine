#pragma once
#include "Archetype.h"
#include "StackArray.h"
#include "Entities/Pickup.h"
#include "Systems/MenuSystem.h"

namespace game
{
	class PickupArchetype final : public Archetype<Pickup>
	{
	public:
		float scalingOnSelected = 0.5f;

		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			jlb::ArrayView<Pickup> entities) override;

	private:
		MenuUpdateInfo _menuUpdateInfo{};
		jlb::StackArray<size_t, 1> _menuInteractIds{};
	};
}
