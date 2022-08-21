#include "pch.h"
#include "Scenes/GameScene.h"

namespace game
{
	void GameScene::Allocate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::Allocate(info, systems);
		auto& tempAllocator = *info.tempAllocator;
		_allocator.Allocate();

		const auto archetypeInfo = CreateInfo(info, systems);
		const auto systemsInitializer = _archetypeManager.CreateInitializer(_allocator, tempAllocator, archetypeInfo);
		DefineSystems(systemsInitializer);
		_archetypeManager.Allocate(_allocator, tempAllocator);

		_archetypeManager.Awake(archetypeInfo);
		_archetypeManager.Start(archetypeInfo);
	}

	void GameScene::Free(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		const auto archetypeInfo = CreateInfo(info, systems);
		_archetypeManager.Exit(archetypeInfo);
		_archetypeManager.Free(_allocator, *info.tempAllocator, archetypeInfo);
		_allocator.Free();
		Scene::Free(info, systems);
	}

	void GameScene::BeginFrame(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::BeginFrame(info, systems);
		_archetypeManager.BeginFrame(CreateInfo(info, systems));
	}

	void GameScene::PostUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::PostUpdate(info, systems);
		_archetypeManager.PostUpdate(CreateInfo(info, systems));
	}

	void GameScene::EndFrame(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::EndFrame(info, systems);
		_archetypeManager.EndFrame(CreateInfo(info, systems));
	}

	void GameScene::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::PreUpdate(info, systems);
		_archetypeManager.PreUpdate(CreateInfo(info, systems));
	}

	jlb::Systems<EntityArchetypeInfo> GameScene::GetEntityArchetypes()
	{
		return _archetypeManager;
	}

	jlb::StackAllocator& GameScene::GetAllocator()
	{
		return _allocator;
	}

	void GameScene::OnKeyInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, 
		const int key, const int action)
	{
		Scene::OnKeyInput(info, systems, key, action);
		_archetypeManager.OnKeyInput(CreateInfo(info, systems), key, action);
	}

	void GameScene::OnMouseInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, 
		const int key, const int action)
	{
		Scene::OnMouseInput(info, systems, key, action);
		_archetypeManager.OnMouseInput(CreateInfo(info, systems), key, action);
	}

	EntityArchetypeInfo GameScene::CreateInfo(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		EntityArchetypeInfo ret{};
		ret.systems = systems;
		ret.sceneAllocator = &_allocator;
		ret.vkeInfo = &info;
		return ret;
	}
}
