#include "VkEngine/pch.h"
#include "VkEngine/Systems/SceneSystem.h"

namespace vke
{
	void SceneSystem::Allocate(const EngineData& info)
	{
		System<EngineData>::Allocate(info);

		Initializer initializer{};
		initializer._sys = this;
		initializer._tempAllocator = info.tempAllocator;
		DefineScenes(initializer);
	}

	void SceneSystem::Load(const size_t id)
	{
		auto& scene = _scenes[id];
		assert(!scene.loaded);
	}

	void SceneSystem::Unload(const size_t id)
	{
		auto& scene = _scenes[id];
		assert(scene.loaded);
	}
}
