#include "pch.h"
#include "Scenes/DemoScene.h"
#include "Systems/CollisionSystem.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Systems/TileRenderSystem.h"

namespace game::demo
{
	void DemoScene::Allocate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::Allocate(info, systems);

		const int32_t dummyCount = 1;
		_dummies.Allocate(*info.allocator, dummyCount * dummyCount);

		for (int32_t i = 0; i < dummyCount; ++i)
		{
			for (int32_t j = 0; j < dummyCount; ++j)
			{
				const auto pos = glm::vec2(i - dummyCount / 2 - dummyCount, j - dummyCount / 2 - dummyCount);
				_dummies[i * dummyCount + j].character.transform.position = pos;
			}
			
		}
	}

	void DemoScene::Free(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		_dummies.Free(*info.allocator);
		Scene::Free(info, systems);
	}

	void DemoScene::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::PreUpdate(info, systems);
		_playerArchetype.PreUpdate(info, systems, _player);
		_dummyArchetype.PreUpdate(info, systems, _dummies);

		const auto collisionSys = systems.GetSystem<CollisionSystem>();
		const auto tileSys = systems.GetSystem<vke::TileRenderSystem>();
		const auto turnSys = systems.GetSystem<TurnSystem>();

		// Tile test.
		vke::TileRenderTask tileTask{};
		tileTask.shape = glm::ivec2(3, 5);
		tileTask.position = glm::ivec2(3, 5);
		auto result = tileSys->TryAdd(info, tileTask);

		if(turnSys->GetIfTickEvent())
		{
			CollisionTask collisionTask{};
			collisionTask.lBot = glm::ivec2(2, 3);
			collisionTask.rTop = collisionTask.lBot + glm::ivec2(2, 4);
			collisionTask.layers = 0b11;
			result = collisionSys->TryAdd(collisionTask);
			assert(result != SIZE_MAX);

			jlb::Bounds b = glm::ivec2(2);
			b.layers = 0b1;
			collisionSys->ReserveTiles(b);
		}
	}

	void DemoScene::PostUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::PostUpdate(info, systems);
		_playerArchetype.PostUpdate(info, systems, _player);
		_dummyArchetype.PostUpdate(info, systems, _dummies);
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
