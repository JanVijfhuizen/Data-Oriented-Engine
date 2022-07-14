#include "pch.h"
#include "Systems/MouseSystem.h"

#include "Systems/ResourceManager.h"
#include "VkEngine/Systems/UIRenderSystem.h"
#include "VkEngine/Systems/EntityRenderSystem.h"

namespace game
{
	void MouseSystem::Allocate(const vke::EngineData& info)
	{
	}

	void MouseSystem::Free(const vke::EngineData& info)
	{
	}

	void MouseSystem::Update(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		if (!info.mouseAvailable)
			return;

		const auto& mousePos = info.mousePos;
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto uiSys = systems.GetSystem<vke::UIRenderSystem>();
		const auto entitySys = systems.GetSystem<vke::EntityRenderSystem>();

		vke::UIRenderTask task{};
		task.transform.position = mousePos;
		task.transform.scale = entitySys->camera.pixelSize * vke::PIXEL_SIZE_ENTITY;
		task.subTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::mouse);

		const auto result = uiSys->TryAdd(task);
		assert(result != SIZE_MAX);
	}
}
