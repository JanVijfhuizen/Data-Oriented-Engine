#include "pch.h"
#include "Systems/MenuSystem.h"

#include "Systems/ResourceManager.h"
#include "Systems/UIInteractionSystem.h"
#include "VkEngine/Graphics/RenderConventions.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	Menu MenuSystem::CreateMenu(const vke::EngineData& info,
		const jlb::Systems<vke::EngineData> systems,
		const MenuCreateInfo& createInfo)
	{
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto uiRenderSys = systems.GetSystem<vke::UIRenderSystem>();
		const auto uiInteractionSys = systems.GetSystem<UIInteractionSystem>();

		const auto& camera = uiRenderSys->camera;
		const float scale = camera.pixelSize * vke::PIXEL_SIZE_ENTITY;

		vke::UIRenderTask renderTask{};
		renderTask.position = createInfo.position;
		renderTask.scale = glm::vec2(createInfo.width * scale, scale);
		renderTask.subTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::blank);
		renderTask.color = glm::vec4(0, 0, 0, 1);
		auto result = uiRenderSys->TryAdd(info, renderTask);
		assert(result != SIZE_MAX);

		UIInteractionTask interactionTask{};
		interactionTask.bounds = jlb::FBounds(renderTask.position, renderTask.scale);
		result = uiInteractionSys->TryAdd(info, interactionTask);
		assert(result != SIZE_MAX);

		Menu menu{};
		return menu;
	}
}
