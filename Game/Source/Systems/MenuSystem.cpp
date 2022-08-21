#include "pch.h"
#include "Systems/MenuSystem.h"
#include "Curve.h"
#include "JlbMath.h"
#include "JlbString.h"
#include "Systems/CardRenderSystem.h"
#include "Systems/ResourceManager.h"
#include "Systems/TextRenderHandler.h"
#include "Systems/UIInteractionSystem.h"
#include "VkEngine/Graphics/Animation.h"
#include "VkEngine/Graphics/RenderConventions.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	size_t MenuCreateInfo::GetColumnCount() const
	{
		return jlb::math::Min<size_t>(maxLength, content.length) - 1;
	}

	size_t MenuCreateInfo::GetInteractedColumnIndex(const MenuUpdateInfo& updateInfo) const
	{
		return updateInfo.interactedIndex == SIZE_MAX ? SIZE_MAX : GetContentIndex(updateInfo, updateInfo.interactedIndex);
	}

	size_t MenuCreateInfo::GetContentIndex(const MenuUpdateInfo& updateInfo, const size_t columnIndex) const
	{
		return (updateInfo.scrollIdx + columnIndex) % (content.length - 1);
	}

	void MenuSystem::CreateMenu(const vke::EngineData& info,
		const jlb::Systems<vke::EngineData> systems,
		const MenuCreateInfo& createInfo, MenuUpdateInfo& updateInfo) const
	{
		const auto resourceSys = systems.Get<ResourceManager>();
		const auto entityRenderSys = systems.Get<vke::EntityRenderSystem>();
		const auto textRenderSys = systems.Get<TextRenderHandler>();
		const auto uiRenderSys = systems.Get<vke::UIRenderSystem>();
		const auto uiInteractSys = systems.Get<UIInteractionSystem>();

		auto& dumpAllocator = *info.dumpAllocator;
		auto& tempAllocator = *info.tempAllocator;

		const auto& camera = entityRenderSys->camera;
		const float scale = camera.pixelSize * vke::PIXEL_SIZE_ENTITY;

		const size_t contentLength = createInfo.content.length;
		const size_t length = jlb::math::Min(contentLength, createInfo.maxLength);
		assert(length > 0);
		assert(createInfo.interactIds.length >= length - 1);

		// Update interactions.
		if (updateInfo.opened)
		{
			const size_t uiHoveredObj = uiInteractSys->GetHoveredObject();
			auto& idx = updateInfo.interactedIndex;
			idx = SIZE_MAX;
			const auto length = createInfo.GetColumnCount();
			for (size_t i = 0; i < length; ++i)
				idx = uiHoveredObj == createInfo.interactIds[i] ? i : idx;
		}

		// Update open duration.
		updateInfo.duration += info.deltaTime * 1e-2f;

		// Calculate screen position for the render task.
		const auto& uiCamera = uiRenderSys->camera;
		auto xOffset = (.5f + static_cast<float>(createInfo.width) * .5f + createInfo.xOffset) * ((camera.position.x > createInfo.origin.x) * 2 - 1);
		xOffset *= -2 * createInfo.reverseXAxis + 1;
		const auto worldPos = createInfo.origin + glm::vec2(xOffset, 0) - entityRenderSys->camera.position;
		const auto screenPos = vke::UIRenderSystem::WorldToScreenPos(worldPos, uiCamera, info.swapChainData->resolution);

		if(updateInfo.right && xOffset < 0 || !updateInfo.right && xOffset > 0)
		{
			updateInfo.right = !updateInfo.right;
			updateInfo.duration = 0;
		}
		
		const auto renderTaskScale = glm::vec2(scale * createInfo.width, scale * length);

		vke::UIRenderTask renderTask{};
		renderTask.position = screenPos;
		renderTask.scale = renderTaskScale;
		renderTask.subTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::blank);

		auto overshooting = jlb::CreateCurveOvershooting();
		const float openLerp = updateInfo.duration / openDuration;
		const float openTextLerp = updateInfo.duration / (openDuration + openWriteTextDuration);
		
		auto& windowHovered = updateInfo.hovered = false;

		// Draw the text and box.
		{
			const float rAspectFix = vke::UIRenderSystem::GetReversedAspectFix(info.swapChainData->resolution);
			const glm::vec2 tabSize{ renderTask.scale.x, renderTask.scale.y / length };
			const float xOffset = scale * (createInfo.width - 1) / 2 * rAspectFix;
			const float yOffset = (renderTask.scale.y - tabSize.y) * .5f;

			// Update scroll position.
			auto& scrollPos = updateInfo.scrollPos;
			{
				const auto& mousePos = info.mousePos;
				const auto& rPos = renderTask.position;
				const auto rScale = renderTask.scale * glm::vec2(rAspectFix, 1);

				jlb::FBounds bounds{rPos, rScale};

				if (bounds.Intersects(mousePos))
				{
					windowHovered = true;

					// Allow for scrolling.
					scrollPos -= _scrollDir;
					scrollPos += scrollPos < 0 ? contentLength : 0;
					scrollPos = fmodf(scrollPos, contentLength);
				}

				const auto worldCenterPos = createInfo.origin + entityRenderSys->camera.position;
				const auto screenCenterPos = vke::UIRenderSystem::WorldToScreenPos(worldCenterPos, uiCamera, info.swapChainData->resolution);
				bounds = { screenCenterPos, rScale };
				updateInfo.centerHovered = bounds.Intersects(mousePos);
			}

			auto& scrollIdx = updateInfo.scrollIdx = roundf(scrollPos);

			renderTask.scale.y /= length + 1;

			const auto origin = screenPos - glm::vec2(xOffset, yOffset + tabSize.y);
			TextRenderTask textTask{};
			textTask.origin = origin;

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
				renderTask.color = glm::vec4(0, 0, 0, 1);

				// Add interaction task.
				if(i > 0)
				{
					UIInteractionTask interactionTask{};
					interactionTask.bounds = jlb::FBounds(glm::vec2(screenPos.x, textTask.origin.y), tabSize * glm::vec2(rAspectFix, 1));
					auto result = uiInteractSys->TryAdd(info, interactionTask);
					assert(result != SIZE_MAX);
					createInfo.interactIds[i - 1] = result;
				}

				auto result = uiRenderSys->TryAdd(info, renderTask);
				assert(result != SIZE_MAX);

				if(i > 0)
				{
					const bool interacted = i - 1 == updateInfo.interactedIndex && updateInfo.opened;
					if (interacted)
					{
						vke::UIRenderTask enabledRenderTask = renderTask;
						enabledRenderTask.color = glm::vec4(0, 0, 0, 1);
						enabledRenderTask.position.x -= camera.pixelSize * 2;
						result = uiRenderSys->TryAdd(info, enabledRenderTask);
						assert(result != SIZE_MAX);
					}

					if(content.interactable)
					{
						renderTask.scale -= glm::vec2(4, 2) * camera.pixelSize;
						renderTask.color = glm::vec4(glm::vec3(1 - interacted), 1);
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

			if(createInfo.usedSpace != SIZE_MAX && createInfo.capacity != SIZE_MAX)
			{
				jlb::String usedSpaceStr{};
				usedSpaceStr.AllocateFromNumber(tempAllocator, createInfo.usedSpace);
				jlb::String capacityStr{};
				capacityStr.AllocateFromNumber(tempAllocator, createInfo.capacity);

				jlb::String str{};
				str.Allocate(dumpAllocator, usedSpaceStr.GetLength() + capacityStr.GetLength());
				const auto strStart = str.GetData();
				memcpy(&strStart[0], usedSpaceStr.GetData(), usedSpaceStr.GetLength() - 1);
				strStart[usedSpaceStr.GetLength() - 1] = '/';
				memcpy(&strStart[usedSpaceStr.GetLength()], capacityStr.GetData(), capacityStr.GetLength() - 1);

				capacityStr.Free(tempAllocator);
				usedSpaceStr.Free(tempAllocator);

				TextRenderTask textTaskAmount{};
				textTaskAmount.text = str;
				textTaskAmount.scale = createInfo.textScale;
				textTaskAmount.padding = static_cast<int32_t>(textTaskAmount.scale) / -2;
				const auto aspectFix = vke::UIRenderSystem::GetAspectFix(info.swapChainData->resolution);
				textTaskAmount.origin = origin;
				auto result = textRenderSys->TryAdd(info, textTaskAmount);
				assert(result != SIZE_MAX);
			}
		}

		// Draw the scroll arrows.
		{
			const int32_t scrollDir = windowHovered ? roundf(_scrollDir) : 0;
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

	void MenuSystem::CreateTextBox(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const TextBoxCreateInfo& createInfo)
	{
		const auto resourceSys = systems.Get<ResourceManager>();
		const auto textRenderSys = systems.Get<TextRenderHandler>();
		const auto uiRenderSys = systems.Get<vke::UIRenderSystem>();

		TextRenderTask cardTextRenderTask{};
		cardTextRenderTask.center = createInfo.center;
		cardTextRenderTask.origin = createInfo.screenOrigin;
		cardTextRenderTask.text = createInfo.text;
		cardTextRenderTask.maxWidth = createInfo.maxWidth;
		cardTextRenderTask.scale = createInfo.scale;
		cardTextRenderTask.padding = static_cast<int32_t>(cardTextRenderTask.scale) / -2;

		const auto& pixelSize = uiRenderSys->camera.pixelSize;
		const auto scale = pixelSize * cardTextRenderTask.scale;
		const auto lineCount = cardTextRenderTask.GetLineCount();
		const auto aspectFix = vke::UIRenderSystem::GetAspectFix(info.swapChainData->resolution);

		vke::UIRenderTask backgroundRenderTask{};
		backgroundRenderTask.position = cardTextRenderTask.origin;
		backgroundRenderTask.scale.x = aspectFix * (scale * cardTextRenderTask.GetWidth());
		backgroundRenderTask.scale.y = scale * lineCount;
		backgroundRenderTask.scale += createInfo.borderSize * pixelSize;
		backgroundRenderTask.color = glm::vec4(0, 0, 0, 1);
		backgroundRenderTask.subTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::blank);
		backgroundRenderTask.position.y += scale * .5f * lineCount - scale * .5f;
		auto result = uiRenderSys->TryAdd(info, backgroundRenderTask);

		result = textRenderSys->TryAdd(info, cardTextRenderTask);
	}

	void MenuSystem::CreateCardMenu(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const CardMenuCreateInfo& createInfo, CardMenuUpdateInfo& updateInfo) const
	{
		const auto cardSys = systems.Get<CardSystem>();
		const auto cardRenderSys = systems.Get<CardRenderSystem>();
		const auto entityRenderSys = systems.Get<vke::EntityRenderSystem>();
		const auto resourceSys = systems.Get<ResourceManager>();
		const auto textRenderSys = systems.Get<TextRenderHandler>();

		auto& dumpAllocator = *info.dumpAllocator;

		const auto cardBorder = resourceSys->GetSubTexture(ResourceManager::CardSubTextures::border);
		const auto& pixelSize = cardRenderSys->camera.pixelSize;

		const auto worldPos = createInfo.origin - entityRenderSys->camera.position;
		const auto screenPos = vke::UIRenderSystem::WorldToScreenPos(worldPos, cardRenderSys->camera, info.swapChainData->resolution);

		vke::UIRenderTask cardRenderTask{};
		cardRenderTask.scale = pixelSize * glm::vec2(static_cast<float>(vke::PIXEL_SIZE_ENTITY * 4));
		cardRenderTask.subTexture = cardBorder;
		cardRenderTask.position = screenPos;
		auto result = cardRenderSys->TryAdd(info, cardRenderTask);

		vke::SubTexture cardSubTexture = resourceSys->GetSubTexture(ResourceManager::CardSubTextures::idle);

		if(createInfo.cardIndex != SIZE_MAX)
		{
			const auto card = cardSys->GetCard(createInfo.cardIndex);
			cardSubTexture = card.art;

			jlb::String str{};
			str.AllocateFromNumber(dumpAllocator, card.cost);

			TextRenderTask textCostTask{};
			textCostTask.center = true;
			textCostTask.origin = screenPos;
			textCostTask.origin.y += cardRenderTask.scale.y * .5f;
			textCostTask.text = str;
			textCostTask.scale = vke::PIXEL_SIZE_ENTITY;
			textCostTask.padding = static_cast<int32_t>(textCostTask.scale) / -2;
			result = textRenderSys->TryAdd(info, textCostTask);

			TextBoxCreateInfo cardTextBox{};
			cardTextBox.text = card.text;
			CreateTextBox(info, systems, cardTextBox);
		}

		auto& animLerp = updateInfo.animLerp;
		animLerp += info.deltaTime * 0.001f * cardAnimSpeed / CARD_ANIM_LENGTH;
		animLerp = fmodf(animLerp, 1);
		vke::Animation cardAnim{};
		cardAnim.lerp = animLerp;
		cardAnim.width = CARD_ANIM_LENGTH;
		auto sub = cardAnim.Evaluate(cardSubTexture, 0);

		cardRenderTask.subTexture = sub;
		result = cardRenderSys->TryAdd(info, cardRenderTask);
	}

	void MenuSystem::OnScrollInput(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,		
		const float xOffset, const float yOffset)
	{
		System<vke::EngineData>::OnScrollInput(info, systems, xOffset, yOffset);
		_scrollDir = yOffset;
	}
}
