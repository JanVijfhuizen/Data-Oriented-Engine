#include "pch.h"
#include "Scenes/GameScene.h"

namespace game
{
	void GameScene::Allocate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::Allocate(info, systems);
		auto& tempAllocator = *info.tempAllocator;
		_sceneAllocator.Allocate();

		const auto archetypeInfo = CreateInfo(info, systems);
		const auto systemsInitializer = _archetypeManager.CreateInitializer(_sceneAllocator, tempAllocator, archetypeInfo);
		DefineSystems(systemsInitializer);
		_archetypeManager.Allocate(_sceneAllocator, tempAllocator);

		_archetypeManager.Awake(archetypeInfo);
		_archetypeManager.Start(archetypeInfo);
	}

	void GameScene::Free(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		const auto archetypeInfo = CreateInfo(info, systems);
		_archetypeManager.Exit(archetypeInfo);
		_archetypeManager.Free(_sceneAllocator, *info.tempAllocator, archetypeInfo);
		_sceneAllocator.Free();
		Scene::Free(info, systems);
	}

	void GameScene::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::PreUpdate(info, systems);
		_archetypeManager.Update(CreateInfo(info, systems));
	}

	EntityArchetypeInfo GameScene::CreateInfo(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		EntityArchetypeInfo ret{};
		ret.systems = systems;
		ret.sceneAllocator = &_sceneAllocator;
		ret.info = &info;
		return ret;
	}
}
