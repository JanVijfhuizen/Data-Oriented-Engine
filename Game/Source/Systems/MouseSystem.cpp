#include "pch.h"
#include "Systems/MouseSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/ResourceManager.h"
#include "VkEngine/Graphics/RenderConventions.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	size_t MouseSystem::GetHoveredObject()
	{
		return _hoveredObject.GetCurrent();
	}

	void MouseSystem::Allocate(const vke::EngineData& info)
	{
		System<vke::EngineData>::Allocate(info);
		for (auto& hovered : _hoveredObject)
			hovered = SIZE_MAX;
	}

	void MouseSystem::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		vke::GameSystem::PreUpdate(info, systems);
		_hoveredObject.Swap();
		_hoveredObject.GetPrevious() = SIZE_MAX;

		if (!info.mouseAvailable)
			return;

		vke::ThreadPoolTask task{};
		task.userPtr = this;
		task.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
		{
			const auto self = reinterpret_cast<MouseSystem*>(userPtr);
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
			task.subTexture = subTextures[self->_pressed];

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
				self->_hoveredObject.GetPrevious() = count == 0 ? SIZE_MAX : intersection;
			}
		};

		const auto threadSys = systems.GetSystem<vke::ThreadPoolSystem>();
		const auto result = threadSys->TryAdd(info, task);
		assert(result != SIZE_MAX);
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
