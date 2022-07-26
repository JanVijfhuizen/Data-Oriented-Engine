#include "pch.h"
#include "Systems/MenuSystem.h"

#include "JlbMath.h"
#include "Systems/ResourceManager.h"
#include "Systems/TextRenderHandler.h"
#include "Systems/UIInteractionSystem.h"
#include "VkEngine/Graphics/RenderConventions.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	Menu MenuSystem::CreateMenu(const vke::EngineData& info,
		const jlb::Systems<vke::EngineData> systems,
		const MenuCreateInfo& createInfo)
	{
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto entityRenderSys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto textRenderSys = systems.GetSystem<TextRenderHandler>();
		const auto uiRenderSys = systems.GetSystem<vke::UIRenderSystem>();
		const auto uiInteractionSys = systems.GetSystem<UIInteractionSystem>();

		const auto& camera = entityRenderSys->camera;
		const float scale = camera.pixelSize * vke::PIXEL_SIZE_ENTITY;

		const size_t length = createInfo.content.length;
		assert(length > 0);

		// Calculate screen position for the render task.
		const auto& uiCamera = uiRenderSys->camera;
		const auto xOffset = (.5f + static_cast<float>(createInfo.width) * .5f) * ((camera.position.x > createInfo.origin.x) * 2 - 1);
		const auto worldPos = createInfo.origin + glm::vec2(xOffset, 0) - entityRenderSys->camera.position;
		const auto screenPos = vke::UIRenderSystem::WorldToScreenPos(worldPos, uiCamera, info.swapChainData->resolution);

		vke::UIRenderTask renderTask{};
		renderTask.position = screenPos;
		renderTask.scale = glm::vec2(scale * createInfo.width, scale * length);
		renderTask.subTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::blank);
		renderTask.color = glm::vec4(0, 0, 0, 1);
		const auto result = uiRenderSys->TryAdd(info, renderTask);
		assert(result != SIZE_MAX);

		// Draw the text.
		{
			const auto tabSize = renderTask.scale.y / length;
			const float yOffset = (renderTask.scale.y - tabSize) * .5f;
			size_t i = 0;

			for (const auto& content : createInfo.content)
			{
				TextRenderTask task{};
				task.origin = screenPos - glm::vec2(0, yOffset - tabSize * i++);
				task.text = content;
				//task.scale = 8;

				const auto result = textRenderSys->TryAdd(info, task);
				assert(result != SIZE_MAX);
			}
		}
		

		/*
		UIInteractionTask interactionTask{};
		interactionTask.bounds = jlb::FBounds(renderTask.position, renderTask.scale);
		result = uiInteractionSys->TryAdd(info, interactionTask);
		assert(result != SIZE_MAX);
		*/
		Menu menu{};
		return menu;
	}
}
