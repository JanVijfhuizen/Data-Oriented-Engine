#include "pch.h"
#include "Systems/MouseSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/ResourceManager.h"
#include "Systems/UIInteractionSystem.h"
#include "VkEngine/Graphics/RenderConventions.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	size_t MouseSystem::GetHoveredObject() const
	{
		return _hoveredObject;
	}

	bool MouseSystem::GetIsPressed(const Key key) const
	{
		return _keys[static_cast<size_t>(key)].pressed;
	}

	bool MouseSystem::GetIsPressedThisTurn(const Key key) const
	{
		return _keys[static_cast<size_t>(key)].pressedThisTurn;
	}

	bool MouseSystem::GetIsUIBlocking() const
	{
		return _isUIBlocking;
	}

	void MouseSystem::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		vke::GameSystem::PreUpdate(info, systems);

		for (auto& key : _keys)
			key.pressedThisTurn = false;
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
		task.subTexture = subTextures[_keys[0].pressed];

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

	void MouseSystem::PostUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		System<vke::EngineData>::PostUpdate(info, systems);

		const auto uiInteractSys = systems.GetSystem<UIInteractionSystem>();
		const size_t uiHoveredObj = uiInteractSys->GetHoveredObject();
		_hoveredObject = uiHoveredObj == SIZE_MAX ? _hoveredObject : SIZE_MAX;
		_isUIBlocking = uiHoveredObj != SIZE_MAX;
	}

	void MouseSystem::OnMouseInput(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems, 
		const int key, const int action)
	{
		System<vke::EngineData>::OnMouseInput(info, systems, key, action);

		for (int i = GLFW_MOUSE_BUTTON_1; i <= GLFW_MOUSE_BUTTON_2; ++i)
		{
			auto& keyData = _keys[i];
			if (key == i)
			{
				const bool pressed = action == GLFW_PRESS;
				keyData.pressedThisTurn = !keyData.pressed && pressed;
				keyData.pressed = pressed ? true : action == GLFW_RELEASE ? false : keyData.pressed;
			}
		}
	}
}
