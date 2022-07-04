#include "pch.h"
#include "Scenes/DemoScene.h"
#include "VkEngine/Systems/EntityRenderSystem.h"

namespace game::demo
{
	void DemoScene::Allocate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		jlb::StackArray<vke::texture::TextureAtlasPartition, 7> partitions{};
		partitions[0].path = "Textures/Green.png";
		partitions[0].width = 2;
		partitions[1].path = "Textures/Atlas-ui.png";
		partitions[1].width = 3;
		partitions[2].path = "Textures/Red.png";
		partitions[3].path = "Textures/Green.png";
		partitions[3].width = 2;
		partitions[4].path = "Textures/Atlas-ui.png";
		partitions[4].width = 3;
		partitions[5].path = "Textures/Red.png";
		partitions[6].path = "Textures/Red.png";
		partitions[7].path = "Textures/Red.png";

		vke::texture::GenerateAtlas(info, "Textures/Atlas.png", "Textures/Coordinates.dat", partitions, 16, 4);

		jlb::StackArray<vke::SubTexture, 7> subTextures{};
		vke::texture::LoadAtlasSubTextures("Textures/Coordinates.dat", subTextures);
		_subTexture = subTextures[0];

		GameScene::Allocate(info, systems);
	}

	void DemoScene::GenerateLevel(const jlb::ArrayView<Tile> level, const vke::EngineData& info,
		jlb::Systems<vke::EngineData> systems) const
	{
		for (size_t i = 0; i < level.length; ++i)
		{
			level[i].subTexture = _subTexture;
		}
	}

	void DemoScene::Update(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems)
	{
		GameScene::Update(info, systems);
	}
}
