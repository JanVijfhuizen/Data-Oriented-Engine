#include "pch.h"
#include "Systems/MouseSystem.h"

#include <iostream>

#include "Systems/CollisionSystem.h"
#include "Systems/ResourceManager.h"
#include "VkEngine/Graphics/RenderConventions.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	void MouseSystem::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		vke::GameSystem::PreUpdate(info, systems);

		if (!info.mouseAvailable)
			return;

		const auto& mousePos = info.mousePos;
		const auto collisionSys = systems.GetSystem<CollisionSystem>();
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto uiSys = systems.GetSystem<vke::UIRenderSystem>();

		vke::UIRenderTask task{};
		task.position = mousePos;
		task.scale = glm::vec2(uiSys->camera.pixelSize * vke::PIXEL_SIZE_ENTITY);
		task.subTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::mouse);

		const auto result = uiSys->TryAdd(info, task);
		assert(result != SIZE_MAX);
	}

	void MouseSystem::OnMouseInput(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems, 
		const int key, const int action)
	{
		System<vke::EngineData>::OnMouseInput(info, systems, key, action);
		if(key == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
		{
			const auto collisionSys = systems.GetSystem<CollisionSystem>();
			const auto entitySys = systems.GetSystem<vke::EntityRenderSystem>();
			const auto uiSys = systems.GetSystem<vke::UIRenderSystem>();

			const auto& resolution = info.swapChainData->resolution;
			uint32_t intersection;
			const auto uiWorldPos = vke::UIRenderSystem::ScreenToWorldPos(info.mousePos, uiSys->camera, resolution);
			auto pos = uiWorldPos + entitySys->camera.position;
			pos = round(pos);
			jlb::Bounds bounds = static_cast<glm::ivec2>(pos);
			std::cout << bounds.lBot.x << " " << bounds.lBot.y << std::endl;
			bounds.layers = collisionLayerInteractable;
			const size_t count = collisionSys->GetIntersections(bounds, intersection);

			// WIP
			if (count > 0)
				std::cout << "eyo" << std::endl;
		}
	}
}
