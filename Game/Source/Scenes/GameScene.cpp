#include "pch.h"
#include "Scenes/GameScene.h"

namespace game
{
	void GameScene::Allocate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::Allocate(info, systems);
		auto& tempAllocator = *info.tempAllocator;
		_allocator.Allocate();
		
		ArchetypeInitializer initializer{};
		initializer._scene = this;
		DefineArchetypes(initializer);
	}

	void GameScene::Free(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		_allocator.Free();
		Scene::Free(info, systems);
	}

	void GameScene::PostUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::PostUpdate(info, systems);
		for (const auto& archetype : _archetypes)
			archetype->PostUpdate();
	}

	void GameScene::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::PreUpdate(info, systems);
		for (const auto& archetype : _archetypes)
			archetype->PreUpdate();
	}

	jlb::StackAllocator& GameScene::GetAllocator()
	{
		return _allocator;
	}

	void GameScene::OnKeyInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, 
		const int key, const int action)
	{
		Scene::OnKeyInput(info, systems, key, action);
		for (const auto& archetype : _archetypes)
			archetype->OnKeyInput(key, action);
	}

	void GameScene::OnMouseInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, 
		const int key, const int action)
	{
		Scene::OnMouseInput(info, systems, key, action);
		for (const auto& archetype : _archetypes)
			archetype->OnMouseInput(key, action);
	}
}
