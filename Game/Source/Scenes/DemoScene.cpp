#include "pch.h"
#include "Scenes/DemoScene.h"

namespace game::demo
{
	void DemoScene::GenerateLevel(const jlb::ArrayView<Tile> level, const vke::EngineData& info,
		jlb::Systems<vke::EngineData> systems) const
	{
		for (size_t i = 0; i < level.length; ++i)
		{
			level[i].walkable = rand() % 4 != 0;
		}
	}
}
