#include "pch.h"
#include "Systems/MenuSystem.h"
#include "Curve.h"
#include "JlbMath.h"
#include "Systems/ResourceManager.h"
#include "Systems/TextRenderHandler.h"
#include "Systems/UIInteractionSystem.h"
#include "VkEngine/Graphics/RenderConventions.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	void MenuUpdateInfo::Reset()
	{
		opened = false;
		duration = 0;
		scrollPos = 0;
	}

	void MenuSystem::CreateMenu(const vke::EngineData& info,
		const jlb::Systems<vke::EngineData> systems,
		const MenuCreateInfo& createInfo, MenuUpdateInfo& updateInfo) const
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

		// Update open duration.
		updateInfo.duration += info.deltaTime * 1e-2f;

		// Calculate screen position for the render task.
		const auto& uiCamera = uiRenderSys->camera;
		const auto xOffset = (.5f + static_cast<float>(createInfo.width) * .5f) * ((camera.position.x > createInfo.origin.x) * 2 - 1);
		const auto worldPos = createInfo.origin + glm::vec2(xOffset, 0) - entityRenderSys->camera.position;
		const auto screenPos = vke::UIRenderSystem::WorldToScreenPos(worldPos, uiCamera, info.swapChainData->resolution);

		if(updateInfo.right && xOffset < 0 || !updateInfo.right && xOffset > 0)
		{
			updateInfo.right = !updateInfo.right;
			updateInfo.duration = 0;
		}

		constexpr auto color = glm::vec4(0, 0, 0, 1);
		constexpr auto interactedColor = glm::vec4(.5f, 0, 0, 1);

		vke::UIRenderTask renderTask{};
		renderTask.position = screenPos;
		renderTask.scale = glm::vec2(scale * createInfo.width, scale * length);
		renderTask.subTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::blank);

		auto overshooting = jlb::CreateCurveOvershooting();
		const float openLerp = updateInfo.duration / openDuration;
		const float openTextLerp = updateInfo.duration / (openDuration + openWriteTextDuration);

		// Draw the text and box.
		{
			const float rAspectFix = vke::UIRenderSystem::GetReversedAspectFix(info.swapChainData->resolution);
			const glm::vec2 tabSize{ renderTask.scale.x * rAspectFix, renderTask.scale.y / length };
			const float xOffset = scale * (createInfo.width - 1) / 2 * rAspectFix;
			const float yOffset = (renderTask.scale.y - tabSize.y) * .5f;

			renderTask.scale.y /= length + 1;

			TextRenderTask task{};
			task.origin = screenPos - glm::vec2(xOffset, yOffset + tabSize.y);
			task.scale = 8;
			task.padding = -4;

			auto& scrollPos = updateInfo.scrollPos;
			int32_t oldScrollPos = roundf(scrollPos);
			scrollPos += _scrollDir;
			scrollPos += scrollPos < 0 ? length : 0;
			scrollPos = fmodf(scrollPos, length);
			const size_t newScrollPos = roundf(scrollPos);

			for (size_t i = 0; i < length; ++i)
			{
				const size_t idx = (newScrollPos + i) % length;

				const auto& content = createInfo.content[idx];
				const float tabDelay = openTabDelay * i;

				task.text = content;
				task.origin.y += tabSize.y;
				renderTask.position.y = task.origin.y;
				renderTask.scale.x *= overshooting.Evaluate(openLerp - tabDelay);
				renderTask.color = i == createInfo.interactedIndex && updateInfo.opened ? interactedColor : color;

				auto result = uiRenderSys->TryAdd(info, renderTask);
				assert(result != SIZE_MAX);

				task.lengthOverride = task.text.GetLength() * jlb::math::Clamp<float>(openTextLerp, 0, 1);
				result = textRenderSys->TryAdd(info, task);
				assert(result != SIZE_MAX);

				UIInteractionTask interactionTask{};
				interactionTask.bounds = jlb::FBounds(glm::vec2(screenPos.x, task.origin.y), tabSize);
				result = uiInteractionSys->TryAdd(info, interactionTask);
				assert(result != SIZE_MAX);

				createInfo.outInteractIds[i] = result;
			}
		}

		updateInfo.opened = true;
	}

	void MenuSystem::PostUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		System<vke::EngineData>::PostUpdate(info, systems);
		_scrollDir = 0;
	}

	void MenuSystem::OnScrollInput(const vke::EngineData& info, 
	    const jlb::Systems<vke::EngineData> systems,		
		const float xOffset, const float yOffset)
	{
		System<vke::EngineData>::OnScrollInput(info, systems, xOffset, yOffset);
		_scrollDir = yOffset;
	}
}
