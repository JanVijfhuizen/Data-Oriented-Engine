#pragma once
#include "Archetypes/DummyArchetype.h"
#include "Archetypes/PickupArchetype.h"
#include "Archetypes/PlayerArchetype.h"
#include "Entities/DummyEntity.h"
#include "Entities/Pickup.h"
#include "VkEngine/Systems/SceneSystem.h"

namespace game::demo
{
	// Silly little scene I use to prototype and test.
	class DemoScene final : public vke::Scene
	{
		jlb::Vector<Player> _players{};
		jlb::Vector<Pickup> _pickups{};
		jlb::Vector<DummyEntity> _dummies{};

		DummyArchetype _dummyArchetype{};
		PlayerArchetype _playerArchetype{};
		PickupArchetype _pickupArchetype{};

		void Allocate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void Free(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;

		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;

		void OnKeyInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, int key,
			int action) override;
		void OnMouseInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, int key,
			int action) override;
	};
}
