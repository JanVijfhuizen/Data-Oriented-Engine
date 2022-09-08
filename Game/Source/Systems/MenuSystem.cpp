#include "pch.h"
#include "Systems/MenuSystem.h"
#include "Curve.h"
#include "JlbMath.h"
#include "JlbString.h"
#include "Systems/ResourceSystem.h"
#include "Systems/TextRenderHandler.h"
#include "Systems/UIInteractionSystem.h"
#include "VkEngine/Graphics/RenderConventions.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	size_t MenuJobUpdateInfo::GetInteractedColumnIndex(const MenuJob& job) const
	{
		return interactedIndex == SIZE_MAX ? SIZE_MAX : GetContentIndex(job, interactedIndex);
	}

	size_t MenuJobUpdateInfo::GetContentIndex(const MenuJob& job, const size_t columnIndex) const
	{
		return (scrollIdx + columnIndex) % (job.content.length - 1);
	}

	size_t MenuJob::GetColumnCount() const
	{
		return jlb::math::Min<size_t>(maxLength, content.length) - 1;
	}

	void MenuSystem::OnPreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<MenuJob>& jobs)
	{
		JobSystemWithOutput<MenuJob, MenuJobUpdateInfo>::OnPreUpdate(info, systems, jobs);

		const auto resourceSys = systems.Get<ResourceSystem>();
		const auto entityRenderSys = systems.Get<vke::EntityRenderSystem>();
		const auto textRenderSys = systems.Get<TextRenderHandler>();
		const auto uiRenderSys = systems.Get<vke::UIRenderSystem>();
		const auto uiInteractSys = systems.Get<UIInteractionSystem>();

		auto& dumpAllocator = *info.dumpAllocator;
		auto& tempAllocator = *info.tempAllocator;

		const auto& camera = entityRenderSys->camera;
		const float scale = camera.pixelSize * vke::PIXEL_SIZE_ENTITY;

		auto& outputs = GetOutputEditable();

		for (auto& job : jobs)
		{
			const size_t contentLength = job.content.length;
			const size_t length = jlb::math::Min(contentLength, job.maxLength);
			assert(length > 0);
			assert(job.interactIds.length >= length - 1);

			auto& updateInfo = job.updateInfo;

			// Update interactions.
			if (updateInfo.opened)
			{
				const size_t uiHoveredObj = uiInteractSys->GetHoveredObject();
				auto& idx = updateInfo.interactedIndex;
				idx = SIZE_MAX;
				const auto length = job.GetColumnCount();
				for (size_t i = 0; i < length; ++i)
					idx = uiHoveredObj == job.interactIds[i] ? i : idx;
			}

			// Update open duration.
			updateInfo.duration += info.deltaTime * 1e-2f;

			// Calculate screen position for the render job.
			const auto& uiCamera = uiRenderSys->camera;
			auto xOffset = (.5f + static_cast<float>(job.width) * .5f + job.xOffset) * ((camera.position.x > job.origin.x) * 2 - 1);
			xOffset *= -2 * job.reverseXAxis + 1;
			const auto worldPos = job.origin + glm::vec2(xOffset, 0) - entityRenderSys->camera.position;
			const auto screenPos = vke::UIRenderSystem::WorldToScreenPos(worldPos, uiCamera, info.swapChainData->resolution);

			if (updateInfo.right && xOffset < 0 || !updateInfo.right && xOffset > 0)
			{
				updateInfo.right = !updateInfo.right;
				updateInfo.duration = 0;
			}

			const auto renderJobScale = glm::vec2(scale * job.width, scale * length);

			vke::UIRenderJob renderJob{};
			renderJob.position = screenPos;
			renderJob.scale = renderJobScale;
			renderJob.subTexture = resourceSys->GetSubTexture(ResourceSystem::UISubTextures::blank);

			auto overshooting = jlb::CreateCurveOvershooting();
			const float openLerp = updateInfo.duration / openDuration;
			const float openTextLerp = updateInfo.duration / (openDuration + openWriteTextDuration);

			auto& windowHovered = updateInfo.hovered = false;

			// Draw the text and box.
			{
				const float rAspectFix = vke::UIRenderSystem::GetReversedAspectFix(info.swapChainData->resolution);
				const glm::vec2 tabSize{ renderJob.scale.x, renderJob.scale.y / length };
				const float xOffset = scale * (job.width - 1) / 2 * rAspectFix;
				const float yOffset = (renderJob.scale.y - tabSize.y) * .5f;

				// Update scroll position.
				auto& scrollPos = updateInfo.scrollPos;
				{
					const auto& mousePos = info.mousePos;
					const auto& rPos = renderJob.position;
					const auto rScale = renderJob.scale * glm::vec2(rAspectFix, 1);

					jlb::FBounds bounds{ rPos, rScale };

					if (bounds.Intersects(mousePos))
					{
						windowHovered = true;

						// Allow for scrolling.
						scrollPos -= _scrollDir;
						scrollPos += scrollPos < 0 ? contentLength : 0;
						scrollPos = fmodf(scrollPos, contentLength);
					}

					const auto worldCenterPos = job.origin + entityRenderSys->camera.position;
					const auto screenCenterPos = vke::UIRenderSystem::WorldToScreenPos(worldCenterPos, uiCamera, info.swapChainData->resolution);
					bounds = { screenCenterPos, rScale };
					updateInfo.centerHovered = bounds.Intersects(mousePos);
				}

				auto& scrollIdx = updateInfo.scrollIdx = static_cast<size_t>(roundf(scrollPos));

				renderJob.scale.y /= static_cast<float>(length + 1);

				const auto origin = screenPos - glm::vec2(xOffset, yOffset + tabSize.y);
				TextRenderJob textJob{};
				textJob.origin = origin;

				for (size_t i = 0; i < length; ++i)
				{
					const size_t idx = i == 0 ? 0 : (scrollIdx + i - 1) % (contentLength - 1) + 1;

					textJob.scale = i == 0 ? job.titleScale : job.textScale;
					textJob.padding = static_cast<int32_t>(textJob.scale) / -2;

					const auto& content = job.content[idx];
					const float tabDelay = openTabDelay * i;

					textJob.text = content.string;
					textJob.origin.y += tabSize.y;

					renderJob.position.y = textJob.origin.y;
					renderJob.scale = tabSize;
					renderJob.scale.x *= overshooting.Evaluate(openLerp - tabDelay);
					renderJob.color = glm::vec4(0, 0, 0, 1);

					// Add interaction job.
					if (i > 0)
					{
						UIInteractionJob interactionJob{};
						interactionJob.bounds = jlb::FBounds(glm::vec2(screenPos.x, textJob.origin.y), tabSize * glm::vec2(rAspectFix, 1));
						auto result = uiInteractSys->TryAdd(info, interactionJob);
						assert(result != SIZE_MAX);
						job.interactIds[i - 1] = result;
					}

					auto result = uiRenderSys->TryAdd(info, renderJob);
					assert(result != SIZE_MAX);

					if (i > 0)
					{
						const bool interacted = i - 1 == updateInfo.interactedIndex && updateInfo.opened;
						if (interacted)
						{
							vke::UIRenderJob enabledRenderJob = renderJob;
							enabledRenderJob.color = glm::vec4(0, 0, 0, 1);
							enabledRenderJob.position.x -= camera.pixelSize * 2;
							result = uiRenderSys->TryAdd(info, enabledRenderJob);
							assert(result != SIZE_MAX);
						}

						if (content.interactable)
						{
							renderJob.scale -= glm::vec2(4, 2) * camera.pixelSize;
							renderJob.color = glm::vec4(glm::vec3(1 - interacted), 1);
							result = uiRenderSys->TryAdd(info, renderJob);
							assert(result != SIZE_MAX);
						}
					}

					textJob.lengthOverride = textJob.text.GetLength() * jlb::math::Clamp<float>(openTextLerp, 0, 1);
					result = textRenderSys->TryAdd(info, textJob);
					assert(result != SIZE_MAX);

					// Draw amount if applicable.
					if (content.amount != SIZE_MAX)
					{
						jlb::String str{};
						str.Allocate(dumpAllocator, "x_");
						str[1] = static_cast<char>(48 + content.amount);

						TextRenderJob textJobAmount = textJob;
						textJobAmount.text = str;
						textJobAmount.lengthOverride = SIZE_MAX;
						textJobAmount.appendIndex = result;
						result = textRenderSys->TryAdd(info, textJobAmount);
						assert(result != SIZE_MAX);
					}
				}

				if (job.usedSpace != SIZE_MAX && job.capacity != SIZE_MAX)
				{
					jlb::String usedSpaceStr{};
					usedSpaceStr.AllocateFromNumber(tempAllocator, job.usedSpace);
					jlb::String capacityStr{};
					capacityStr.AllocateFromNumber(tempAllocator, job.capacity);

					jlb::String str{};
					str.Allocate(dumpAllocator, usedSpaceStr.GetLength() + capacityStr.GetLength());
					const auto strStart = str.GetData();
					memcpy(&strStart[0], usedSpaceStr.GetData(), usedSpaceStr.GetLength() - 1);
					strStart[usedSpaceStr.GetLength() - 1] = '/';
					memcpy(&strStart[usedSpaceStr.GetLength()], capacityStr.GetData(), capacityStr.GetLength() - 1);

					capacityStr.Free(tempAllocator);
					usedSpaceStr.Free(tempAllocator);

					TextRenderJob textJobAmount{};
					textJobAmount.text = str;
					textJobAmount.scale = job.textScale;
					textJobAmount.padding = static_cast<int32_t>(textJobAmount.scale) / -2;
					const auto aspectFix = vke::UIRenderSystem::GetAspectFix(info.swapChainData->resolution);
					textJobAmount.origin = origin;
					auto result = textRenderSys->TryAdd(info, textJobAmount);
					assert(result != SIZE_MAX);
				}
			}
			
			// Draw the scroll arrows.
			{
				const int32_t scrollDir = windowHovered ? static_cast<int32_t>(roundf(_scrollDir)) : 0;
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

				auto arrowSubTexture = resourceSys->GetSubTexture(ResourceSystem::UISubTextures::scrollArrow);
				jlb::StackArray<vke::SubTexture, 2> arrows{};
				vke::texture::Subdivide(arrowSubTexture, 2, arrows);

				const glm::vec2 arrowOffset{ 0, renderJobScale.y / 2 };
				const auto arrowScale = glm::vec2(scale);
				vke::UIRenderJob arrowRenderJob{};

				arrowRenderJob.position = screenPos - arrowOffset;
				arrowRenderJob.scale = arrowScale * (1.f + jlb::DoubleCurveEvaluate(updateInfo.scrollArrowsLerp[0],
					overshootingCurve, decelerateCurve) * scrollAnimScaleMultiplier);
				arrowRenderJob.subTexture = arrows[0];
				auto result = uiRenderSys->TryAdd(info, arrowRenderJob);
				assert(result != SIZE_MAX);

				arrowRenderJob.position = screenPos + arrowOffset;
				arrowRenderJob.scale = arrowScale * (1.f + jlb::DoubleCurveEvaluate(updateInfo.scrollArrowsLerp[1],
					overshootingCurve, decelerateCurve) * scrollAnimScaleMultiplier);
				arrowRenderJob.subTexture = arrows[1];
				result = uiRenderSys->TryAdd(info, arrowRenderJob);
				assert(result != SIZE_MAX);
			}

			updateInfo.opened = true;

			outputs.Add(dumpAllocator, job.updateInfo);
		}
	}

	void MenuSystem::OnPostUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<MenuJob>& tasks)
	{
		JobSystemWithOutput<MenuJob, MenuJobUpdateInfo>::OnPostUpdate(info, systems, tasks);
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
