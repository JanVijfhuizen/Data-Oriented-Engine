#include "pch.h"
#include "Systems/MouseSystem.h"
#include "Systems/ResourceManager.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	void MouseSystem::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		vke::GameSystem::PreUpdate(info, systems);

		if (!info.mouseAvailable)
			return;

		const auto& mousePos = info.mousePos;
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto uiSys = systems.GetSystem<vke::UIRenderSystem>();

		vke::UIRenderTask task{};
		task.transform.position = mousePos;
		task.transform.scale = uiSys->camera.pixelSize * vke::PIXEL_SIZE_ENTITY;
		task.subTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::mouse);

		const auto result = uiSys->TryAdd(info, task);
		assert(result != SIZE_MAX);
	}
}
