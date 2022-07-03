#pragma once
#include "GameScene.h"

namespace game::demo
{
	class DemoScene final : public GameScene
	{
		void Update(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
	};
}
