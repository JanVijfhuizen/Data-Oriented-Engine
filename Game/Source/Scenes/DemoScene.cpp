#include "pch.h"
#include "Scenes/DemoScene.h"

#include <iostream>

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

		jlb::StackArray<glm::vec2, 16> in{};
		jlb::DistanceTree disTree{};
		disTree.Allocate(*info.dumpAllocator, 16);

		srand(time(NULL));
		for (int i = 0; i < 16; ++i)
		{
			glm::vec2 v = glm::vec2(rand() % 100 - 50, rand() % 100 - 50);
			disTree.Add(v);
			in[i] = v;
		}

		jlb::StackArray<size_t, 16> out{};
		size_t c = disTree.GetInstancesInRange(glm::vec2(0), 15, out);
		for (int i = 0; i < c; ++i)
		{
			glm::vec2 vec = in[out[i]];
			std::cout << vec.x << " " << vec.y << std::endl;
		}

		std::cout << std::endl;

		for (auto& vec : in)
		{
			std::cout << vec.x << " " << vec.y << std::endl;
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
