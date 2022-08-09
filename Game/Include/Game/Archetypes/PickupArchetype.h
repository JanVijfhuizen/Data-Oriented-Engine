﻿#pragma once
#include "EntityArchetype.h"
#include "StackArray.h"
#include "Entities/Pickup.h"
#include "Systems/MenuSystem.h"

namespace game
{
	class PickupArchetype final : public EntityArchetype<Pickup>
	{
	public:
		float scalingOnSelected = 0.5f;

		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			jlb::Vector<Pickup>& entities) override;

	private:
		MenuUpdateInfo _menuUpdateInfo{};
		CardMenuUpdateInfo _cardMenuUpdateInfo{};
		jlb::StackArray<size_t, 1> _menuInteractIds{};
	};
}