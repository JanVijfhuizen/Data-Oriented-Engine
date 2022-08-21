#pragma once
#include "EntityArchetype.h"
#include "StackArray.h"
#include "Entities/Pickup.h"
#include "Systems/MenuSystem.h"

namespace game
{
	class PickupArchetype final : public EntityArchetype<Pickup>
	{
	protected:
		const float scalingOnSelected = 0.5f;

		void OnPreUpdate(const EntityArchetypeInfo& info, jlb::Systems<EntityArchetypeInfo> archetypes,
			jlb::NestedVector<Pickup>& entities) override;
	private:
		MenuUpdateInfo _menuUpdateInfo{};
		CardMenuUpdateInfo _cardMenuUpdateInfo{};
		jlb::StackArray<size_t, 1> _menuInteractIds{};
	};
}
