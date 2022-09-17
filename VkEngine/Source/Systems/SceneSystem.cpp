#include "VkEngine/pch.h"
#include "VkEngine/Systems/SceneSystem.h"

namespace vke
{
	void SceneSystem::Allocate(const EngineData& info)
	{
		System<EngineData>::Allocate(info);

		const auto systemsInitializer = _sceneManager.CreateInitializer(*info.allocator, *info.tempAllocator, info);
		DefineScenes(systemsInitializer);

		_sceneManager.Allocate(*info.allocator, *info.tempAllocator);
	}

	void SceneSystem::Free(const EngineData& info)
	{
		_sceneManager.Free(*info.allocator, *info.tempAllocator, info);
		System<EngineData>::Free(info);
	}

	void SceneSystem::Awake(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		System<EngineData>::Awake(info, systems);
		_sceneManager.Awake(info);
	}

	void SceneSystem::Start(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		System<EngineData>::Start(info, systems);
		_sceneManager.Start(info);
	}

	void SceneSystem::BeginFrame(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		System<EngineData>::BeginFrame(info, systems);
		_sceneManager.BeginFrame(info);
	}

	void SceneSystem::PreUpdate(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		System<EngineData>::PreUpdate(info, systems);
		_sceneManager.PreUpdate(info);
	}

	void SceneSystem::Update(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		System<EngineData>::Update(info, systems);
		_sceneManager.Update(info);
	}

	void SceneSystem::PostUpdate(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		System<EngineData>::PostUpdate(info, systems);
		_sceneManager.PostUpdate(info);
	}

	void SceneSystem::EndFrame(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		System<EngineData>::EndFrame(info, systems);
		_sceneManager.EndFrame(info);
	}

	void SceneSystem::OnRecreateSwapChainAssets(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		System<EngineData>::OnRecreateSwapChainAssets(info, systems);
		_sceneManager.OnRecreateSwapChainAssets(info);
	}

	void SceneSystem::OnKeyInput(const EngineData& info, const jlb::Systems<EngineData> systems, 
		const int key, const int action)
	{
		System<EngineData>::OnKeyInput(info, systems, key, action);
		_sceneManager.OnKeyInput(info, key, action);
	}

	void SceneSystem::OnMouseInput(const EngineData& info, const jlb::Systems<EngineData> systems, 
		const int key, const int action)
	{
		System<EngineData>::OnMouseInput(info, systems, key, action);
		_sceneManager.OnMouseInput(info, key, action);
	}

	void SceneSystem::OnScrollInput(const EngineData& info, const jlb::Systems<EngineData> systems, 
		const float xOffset, const float yOffset)
	{
		System<EngineData>::OnScrollInput(info, systems, xOffset, yOffset);
		_sceneManager.OnScrollInput(info, xOffset, yOffset);
	}

	void SceneSystem::Exit(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		System<EngineData>::Exit(info, systems);
		_sceneManager.Exit(info);
	}
}
