#pragma once
#include "GameScene.h"

namespace game::demo
{
	class DemoScene final : public GameScene
	{
		vke::SubTexture _subTexture;

		void Allocate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void GenerateLevel(jlb::ArrayView<Tile> level, const vke::EngineData& info,
			jlb::Systems<vke::EngineData> systems) const override;
		void Update(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
	};
}
