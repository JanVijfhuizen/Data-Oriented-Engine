#include "pch.h"
#include "Scenes/DemoScene.h"
#include "VkEngine/Systems/UIRenderSystem.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include <iostream>

namespace game::demo
{
	void DemoScene::Allocate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		GameScene::Allocate(info, systems);

		jlb::StackArray<vke::texture::TextureAtlasPartition, 3> partitions{};
		partitions[0].path = "Textures/Atlas.png";
		partitions[0].width = 2;
		partitions[1].path = "Textures/Atlas-ui.png";
		partitions[1].width = 2;
		partitions[2].path = "Textures/Red.png";
		jlb::StackArray<vke::SubTexture, 3> outSubTextures{};
		vke::texture::LoadAsAtlas(info, partitions, outSubTextures, glm::ivec2(16), 4);
	}

	void DemoScene::GenerateLevel(const jlb::ArrayView<Tile> level, const vke::EngineData& info,
		jlb::Systems<vke::EngineData> systems) const
	{
		for (size_t i = 0; i < level.length; ++i)
		{
			level[i].walkable = rand() % 4 != 0;
		}
	}

	void DemoScene::Update(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems)
	{
		GameScene::Update(info, systems);

		const auto sys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto worldPos = vke::UIRenderSystem::ScreenToWorldPos(info.mousePos, sys->camera);
		const auto grid = GetGrid();

		std::cout << worldPos.x << " " << worldPos.y << std::endl;

		const size_t index = ToGridIndex(worldPos);
		if(index != SIZE_MAX)
		{
			grid[index].walkable = false;
		}
	}
}
