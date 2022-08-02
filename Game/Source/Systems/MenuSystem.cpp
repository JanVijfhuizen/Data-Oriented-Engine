#include "pch.h"
#include "Systems/MenuSystem.h"
#include "Curve.h"
#include "JlbMath.h"
#include "JlbString.h"
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
		scrollIdx = 0;
		scrollPos = 0;
		for (float& lerp : scrollArrowsLerp)
			lerp = 1;
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

		auto& dumpAllocator = *info.dumpAllocator;
		const auto& camera = entityRenderSys->camera;
		const float scale = camera.pixelSize * vke::PIXEL_SIZE_ENTITY;

		const size_t contentLength = createInfo.content.length;
		const size_t length = jlb::math::Min(contentLength, createInfo.maxLength);
		assert(length > 1);
		assert(createInfo.outInteractIds.length >= length - 1);

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
		const auto renderTaskScale = glm::vec2(scale * createInfo.width, scale * length);

		vke::UIRenderTask renderTask{};
		renderTask.position = screenPos;
		renderTask.scale = renderTaskScale;
		renderTask.subTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::blank);

		auto& scrollPos = updateInfo.scrollPos;
		scrollPos -= _scrollDir;
		scrollPos += scrollPos < 0 ? contentLength : 0;
		scrollPos = fmodf(scrollPos, contentLength);
		auto& scrollIdx = updateInfo.scrollIdx = roundf(scrollPos);

		auto overshooting = jlb::CreateCurveOvershooting();
		const float openLerp = updateInfo.duration / openDuration;
		const float openTextLerp = updateInfo.duration / (openDuration + openWriteTextDuration);

		// Draw the text and box.
		{
			const float rAspectFix = vke::UIRenderSystem::GetReversedAspectFix(info.swapChainData->resolution);
			const glm::vec2 tabSize{ renderTask.scale.x, renderTask.scale.y / length };
			const float xOffset = scale * (createInfo.width - 1) / 2 * rAspectFix;
			const float yOffset = (renderTask.scale.y - tabSize.y) * .5f;

			renderTask.scale.y /= length + 1;

			TextRenderTask textTask{};
			textTask.origin = screenPos - glm::vec2(xOffset, yOffset + tabSize.y);

			for (size_t i = 0; i < length; ++i)
			{
				const size_t idx = i == 0 ? 0 : (scrollIdx + i - 1) % (contentLength - 1) + 1;

				textTask.scale = i == 0 ? createInfo.titleScale : createInfo.textScale;
				textTask.padding = static_cast<int32_t>(textTask.scale) / -2;

				const auto& content = createInfo.content[idx];
				const float tabDelay = openTabDelay * i;

				textTask.text = content.string;
				textTask.origin.y += tabSize.y;

				renderTask.position.y = textTask.origin.y;
				renderTask.scale = tabSize;
				renderTask.scale.x *= overshooting.Evaluate(openLerp - tabDelay);
				renderTask.color = color;

				// Add interaction task.
				if(i > 0)
				{
					UIInteractionTask interactionTask{};
					interactionTask.bounds = jlb::FBounds(glm::vec2(screenPos.x, textTask.origin.y), tabSize * glm::vec2(rAspectFix, 1));
					auto result = uiInteractionSys->TryAdd(info, interactionTask);
					assert(result != SIZE_MAX);
					createInfo.outInteractIds[i - 1] = result;
				}

				auto result = uiRenderSys->TryAdd(info, renderTask);
				assert(result != SIZE_MAX);

				if(i > 0)
				{
					const bool interacted = i - 1 == createInfo.interactedIndex && updateInfo.opened;

					if (interacted)
					{
						vke::UIRenderTask enabledRenderTask = renderTask;
						enabledRenderTask.color = glm::vec4(0, 0, 0, 1);
						enabledRenderTask.position.x -= camera.pixelSize * 2;
						result = uiRenderSys->TryAdd(info, enabledRenderTask);
						assert(result != SIZE_MAX);
					}

					if(content.active)
					{
						renderTask.scale -= glm::vec2(4, 2) * camera.pixelSize;
						renderTask.color = glm::vec4(1);
						result = uiRenderSys->TryAdd(info, renderTask);
						assert(result != SIZE_MAX);
					}
				}

				textTask.lengthOverride = textTask.text.GetLength() * jlb::math::Clamp<float>(openTextLerp, 0, 1);
				result = textRenderSys->TryAdd(info, textTask);
				assert(result != SIZE_MAX);

				// Draw amount if applicable.
				if(content.amount != SIZE_MAX)
				{
					jlb::String str{};
					str.Allocate(dumpAllocator, "x_");
					str[1] = static_cast<char>(48 + content.amount);

					TextRenderTask textTaskAmount = textTask;
					textTaskAmount.text = str;
					textTaskAmount.lengthOverride = SIZE_MAX;
					textTaskAmount.appendIndex = result;
					result = textRenderSys->TryAdd(info, textTaskAmount);
					assert(result != SIZE_MAX);
				}
			}
		}

		// Draw the scroll arrows.
		{
			const int32_t scrollDir = roundf(_scrollDir);
			auto overshootingCurve = jlb::CreateCurveOvershooting();
			auto decelerateCurve = jlb::CreateCurveDecelerate();

			const float scrollSpeed = 1.f / scrollAnimDuration;
			for (int32_t i = 0; i < 2; ++i)
			{
				auto& lerp = updateInfo.scrollArrowsLerp[1 - i];
				lerp = scrollDir == i * 2 - 1 ? 0 : lerp;
				lerp += info.deltaTime * 1e-2f * scrollSpeed;
				lerp = jlb::math::Min<float>(lerp, 1);
			}

			auto arrowSubTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::scrollArrow);
			jlb::StackArray<vke::SubTexture, 2> arrows{};
			vke::texture::Subdivide(arrowSubTexture, 2, arrows);

			const glm::vec2 arrowOffset{ 0, renderTaskScale.y / 2 };
			const auto arrowScale = glm::vec2(scale);
			vke::UIRenderTask arrowRenderTask{};

			arrowRenderTask.position = screenPos - arrowOffset;
			arrowRenderTask.scale = arrowScale * (1.f + jlb::DoubleCurveEvaluate(updateInfo.scrollArrowsLerp[0], 
				overshootingCurve, decelerateCurve) * scrollAnimScaleMultiplier);
			arrowRenderTask.subTexture = arrows[0];
			auto result = uiRenderSys->TryAdd(info, arrowRenderTask);
			assert(result != SIZE_MAX);

			arrowRenderTask.position = screenPos + arrowOffset;
			arrowRenderTask.scale = arrowScale * (1.f + jlb::DoubleCurveEvaluate(updateInfo.scrollArrowsLerp[1], 
				overshootingCurve, decelerateCurve) * scrollAnimScaleMultiplier);
			arrowRenderTask.subTexture = arrows[1];
			result = uiRenderSys->TryAdd(info, arrowRenderTask);
			assert(result != SIZE_MAX);
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
