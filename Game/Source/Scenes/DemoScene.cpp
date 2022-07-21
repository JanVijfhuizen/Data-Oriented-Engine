﻿#include "pch.h"
#include "Scenes/DemoScene.h"
#include "Systems/CollisionSystem.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Systems/TileRenderSystem.h"

namespace game::demo
{
	void DemoScene::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::PreUpdate(info, systems);
		_playerArchetype.PreUpdate(info, systems, _player);

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
			collisionTask.position = tileTask.position;
			collisionTask.scale = tileTask.shape;
			result = collisionSys->TryAdd(collisionTask);
			assert(result != SIZE_MAX);
		}
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
