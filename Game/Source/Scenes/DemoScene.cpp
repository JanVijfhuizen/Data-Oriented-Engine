#include "pch.h"
#include "Scenes/DemoScene.h"
#include "VkEngine/Systems/TileRenderSystem.h"

namespace game::demo
{
	void DemoScene::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::PreUpdate(info, systems);
		_playerArchetype.PreUpdate(info, systems, _player);

		// Tile test.
		const auto tileSys = systems.GetSystem<vke::TileRenderSystem>();
		vke::TileRenderTask task{};
		task.shape = glm::ivec2(3, 5);
		const auto result = tileSys->TryAdd(info, task);
	}

	void DemoScene::PostUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::PostUpdate(info, systems);
	}

	void DemoScene::EndFrame(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::EndFrame(info, systems);
		_playerArchetype.EndFrame(info, systems, _player);
	}

	void DemoScene::OnKeyInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, 
		const int key, const int action)
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
