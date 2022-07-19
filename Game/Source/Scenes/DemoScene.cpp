#include "pch.h"
#include "Scenes/DemoScene.h"
#include "Systems/GameManager.h"
#include "VkEngine/Systems/TileRenderSystem.h"

namespace game::demo
{
	void DemoScene::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::PreUpdate(info, systems);
		_playerArchetype.PreUpdate(info, systems, _player);

		const auto tileSys = systems.GetSystem<vke::TileRenderSystem>();
		for (int i = -10; i < 10; ++i)
			for (int j = -10; j < 10; ++j)
			{
				vke::TileRenderTask task{};
				task.position = glm::vec2(i, j) * static_cast<float>(vke::PIXEL_SIZE_ENTITY);
				const auto result = tileSys->TryAdd(info, task);
				assert(result != SIZE_MAX);
			}
	}

	void DemoScene::PostUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::PostUpdate(info, systems);
		_playerArchetype.PostUpdate(info, systems, _player);
	}

	void DemoScene::OnKeyInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, const int key,
		const int action)
	{
		Scene::OnKeyInput(info, systems, key, action);
		_playerArchetype.OnKeyInput(info, systems, key, action);
	}

	void DemoScene::OnMouseInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems,
		const int key, const int action)
	{
		Scene::OnMouseInput(info, systems, key, action);
		_playerArchetype.OnMouseInput(info, systems, key, action);
	}
}
