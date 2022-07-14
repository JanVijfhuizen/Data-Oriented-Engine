#pragma once
#include "GameScene.h"
#include "Archetypes/PlayerArchetype.h"

namespace game::demo
{
	class DemoScene final : public GameScene
	{
		Player _player{};
		PlayerArchetype _playerArchetype{};
		
		void GenerateLevel(jlb::ArrayView<Tile> level, const vke::EngineData& info,
			jlb::Systems<vke::EngineData> systems) const override;
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
	};
}
