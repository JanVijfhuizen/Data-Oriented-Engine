#pragma once
#include "Archetypes/PlayerArchetype.h"
#include "VkEngine/Systems/SceneSystem.h"

namespace game::demo
{
	class DemoScene final : public vke::Scene
	{
		Player _player{};
		PlayerArchetype _playerArchetype{};

		void Allocate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void Free(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;

		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
	};
}
