#include "pch.h"
#include "Scenes/DemoScene.h"

#include <ctime>

#include "BVH.h"
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

		srand(time(NULL));
		const size_t S = 25;
		jlb::StackArray<jlb::BoundingVolumeHierarchy::Instance, S> vs{};
		for (int i = 0; i < S; ++i)
		{
			vs[i].position = glm::vec2(rand() % 50 - 25, rand() % 50 - 25);
		}

		jlb::BoundingVolumeHierarchy bvh{};
		bvh.Allocate(*info.allocator, vs);
		bvh.Free(*info.allocator);
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
