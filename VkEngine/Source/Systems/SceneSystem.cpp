#include "VkEngine/pch.h"
#include "VkEngine/Systems/SceneSystem.h"

namespace vke
{
	void SceneSystem::PopScene(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		assert(_latestScene);
		Scene* deleteableScene = _latestScene;
		const jlb::AllocationID allocationId = deleteableScene->_allocationId;
		_latestScene = deleteableScene->_next;
		
		deleteableScene->Free(info, systems);
		info.allocator->MFree(allocationId);
	}

	void SceneSystem::Allocate(const EngineData& info)
	{
	}

	void SceneSystem::Free(const EngineData& info)
	{
	}

	void SceneSystem::PreUpdate(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		System<EngineData>::PreUpdate(info, systems);
		Scene* current = _latestScene;
		while (current)
		{
			current->PreUpdate(info, systems);
			current = current->_next;
		}
	}

	void SceneSystem::PostUpdate(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		System<EngineData>::PostUpdate(info, systems);
		Scene* current = _latestScene;
		while (current)
		{
			current->PostUpdate(info, systems);
			current = current->_next;
		}
	}
}
