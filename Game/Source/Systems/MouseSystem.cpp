#include "pch.h"
#include "Systems/MouseSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/ResourceManager.h"
#include "VkEngine/Graphics/RenderConventions.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	size_t MouseSystem::GetHoveredObject() const
	{
		return _hoveredObject;
	}

	void MouseSystem::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		vke::GameSystem::PreUpdate(info, systems);

		if (!info.mouseAvailable)
			return;

		const auto& mousePos = info.mousePos;
		const auto collisionSys = systems.GetSystem<CollisionSystem>();
		const auto entitySys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto uiSys = systems.GetSystem<vke::UIRenderSystem>();

		const auto subTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::mouse);
		jlb::StackArray<vke::SubTexture, 2> subTextures{};
		vke::texture::Subdivide(subTexture, 2, subTextures);

		vke::UIRenderTask task{};
		task.position = mousePos;
		task.scale = glm::vec2(uiSys->camera.pixelSize * vke::PIXEL_SIZE_ENTITY);
		task.subTexture = subTextures[_pressed];

		const auto result = uiSys->TryAdd(info, task);
		assert(result != SIZE_MAX);

		// Try and interact with objects.
		{
			const auto& resolution = info.swapChainData->resolution;
			uint32_t intersection;
			const auto uiWorldPos = vke::UIRenderSystem::ScreenToWorldPos(info.mousePos, uiSys->camera, resolution);
			auto pos = uiWorldPos + entitySys->camera.position;
			pos = round(pos);
			jlb::Bounds bounds = static_cast<glm::ivec2>(pos);
			bounds.layers = collisionLayerInteractable;
			const size_t count = collisionSys->GetIntersections(bounds, intersection);
			_hoveredObject = count == 0 ? SIZE_MAX : intersection;
		}
	}

	void MouseSystem::OnMouseInput(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems, 
		const int key, const int action)
	{
		System<vke::EngineData>::OnMouseInput(info, systems, key, action);
		if(key == GLFW_MOUSE_BUTTON_1)
			_pressed = action == GLFW_PRESS ? true : action == GLFW_RELEASE ? false : _pressed;
	}
}
