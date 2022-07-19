﻿#include "pch.h"
#include "Scenes/DemoScene.h"

#include "DistanceTree.h"
#include "Systems/GameManager.h"
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
		assert(result != SIZE_MAX);

		jlb::DistanceTree disTree{};
		disTree.Allocate(*info.dumpAllocator, 16);

		for (int i = 0; i < 16; ++i)
		{
			//disTree.Add(glm::vec2(rand() % 100 - 50, rand() % 100 - 50));
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
