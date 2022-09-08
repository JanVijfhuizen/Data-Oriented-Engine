#include "pch.h"
#include "Systems/MouseSystem.h"
#include "Systems/ResourceSystem.h"
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

	void MouseSystem::OnPreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<jlb::TBounds<float>>& tasks)
	{
		JobSystem<jlb::TBounds<float>>::OnPreUpdate(info, systems, tasks);

		for (auto& key : _keys)
			key.pressedThisTurn = false;
		if (!info.mouseAvailable)
			return;

		const auto& mousePos = info.mousePos;
		const auto entitySys = systems.Get<vke::EntityRenderSystem>();
		const auto resourceSys = systems.Get<ResourceSystem>();
		const auto uiSys = systems.Get<vke::UIRenderSystem>();

		const auto subTexture = resourceSys->GetSubTexture(ResourceSystem::UISubTextures::mouse);
		jlb::StackArray<vke::SubTexture, 2> subTextures{};
		vke::texture::Subdivide(subTexture, 2, subTextures);

		vke::UIRenderJob uiRenderTask{};
		uiRenderTask.position = mousePos;
		uiRenderTask.scale = glm::vec2(uiSys->camera.pixelSize * vke::PIXEL_SIZE_ENTITY);
		uiRenderTask.subTexture = subTextures[_keys[0].pressed];

		const auto result = uiSys->TryAdd(info, uiRenderTask);
		assert(result != SIZE_MAX);

		// Try and interact with objects.
		const auto& resolution = info.swapChainData->resolution;
		const auto uiWorldPos = vke::UIRenderSystem::ScreenToWorldPos(info.mousePos, uiSys->camera, resolution);
		auto pos = uiWorldPos + entitySys->camera.position;

		_hoveredObject = SIZE_MAX;
		size_t i = 0;

		for (const auto& task : tasks)
		{
			_hoveredObject = task.Intersects(pos) ? i : _hoveredObject;
			i++;
		}
	}

	void MouseSystem::OnPostUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<jlb::TBounds<float>>& tasks)
	{
		JobSystem<jlb::TBounds<float>>::OnPostUpdate(info, systems, tasks);

		const auto uiInteractSys = systems.Get<UIInteractionSystem>();
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
