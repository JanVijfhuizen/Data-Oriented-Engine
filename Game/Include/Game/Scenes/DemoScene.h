#pragma once
#include "GameScene.h"

namespace game::demo
{
	// Silly little scene I use to prototype and test.
	class DemoScene final : public GameScene
	{
		void Allocate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void DefineArchetypes(ArchetypeInitializer& initializer) override;
	};
}
