#include "pch.h"
#include "Scenes/DemoScene.h"

#include "TextRenderHandler.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "Systems/ResourceManager.h"

namespace game::demo
{
	void DemoScene::GenerateLevel(const jlb::ArrayView<Tile> level, const vke::EngineData& info,
		jlb::Systems<vke::EngineData> systems) const
	{
		const auto sys = systems.GetSystem<ResourceManager>();
		const auto subTexture = sys->GetSubTexture(ResourceManager::EntitySubTextures::tile);

		for (size_t i = 0; i < level.length; ++i)
		{
			level[i].subTexture = subTexture;
		}
	}

	void DemoScene::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		GameScene::PreUpdate(info, systems);

		const auto textRenderHandler = systems.GetSystem<TextRenderHandler>();
		TextRenderTask task{};
		task.text = "test 1";
		auto result = textRenderHandler->TryAdd(task);
		assert(result != SIZE_MAX);

		task.appendIndex = result;
		result = textRenderHandler->TryAdd(task);
		assert(result != SIZE_MAX);

		_playerArchetype.PreUpdate(info, systems, _player);
	}

	void DemoScene::PostUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		GameScene::PostUpdate(info, systems);
		_playerArchetype.PostUpdate(info, systems, _player);
	}
}
