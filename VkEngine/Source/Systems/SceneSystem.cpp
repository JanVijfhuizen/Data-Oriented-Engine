﻿#include "VkEngine/pch.h"
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

	void SceneSystem::Update(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		Scene* current = _latestScene;
		while(current)
		{
			current->Update(info, systems);
			current = current->_next;
		}
	}
}