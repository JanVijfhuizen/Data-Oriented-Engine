#include "pch.h"
#include "Systems/CardPreviewSystem.h"
#include "JlbString.h"
#include "Systems/CardRenderSystem.h"
#include "Systems/CardSystem.h"
#include "Systems/ResourceSystem.h"
#include "Systems/TextBoxSystem.h"
#include "Systems/TextRenderHandler.h"
#include "VkEngine/Graphics/Animation.h"
#include "VkEngine/Systems/EntityRenderSystem.h"

namespace game
{
	void CardPreviewSystem::OnPreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<CardPreviewJob>& jobs)
	{
		JobSystemWithOutput<CardPreviewJob, CardPreviewJobUpdateInfo>::OnPreUpdate(info, systems, jobs);

		const auto cardSys = systems.Get<CardSystem>();
		const auto cardRenderSys = systems.Get<CardRenderSystem>();
		const auto entityRenderSys = systems.Get<vke::EntityRenderSystem>();
		const auto resourceSys = systems.Get<ResourceSystem>();
		const auto textBoxSys = systems.Get<TextBoxSystem>();
		const auto textRenderSys = systems.Get<TextRenderHandler>();

		auto& outputs = GetOutputEditable();

		for (auto& job : jobs)
		{
			auto& dumpAllocator = *info.dumpAllocator;

			const auto cardBorder = resourceSys->GetSubTexture(ResourceSystem::CardSubTextures::border);
			const auto& pixelSize = cardRenderSys->camera.pixelSize;

			const auto worldPos = job.origin - entityRenderSys->camera.position;
			const auto screenPos = vke::UIRenderSystem::WorldToScreenPos(worldPos, cardRenderSys->camera, info.swapChainData->resolution);

			vke::UIRenderJob cardRenderJob{};
			cardRenderJob.scale = pixelSize * glm::vec2(static_cast<float>(vke::PIXEL_SIZE_ENTITY * 4));
			cardRenderJob.subTexture = cardBorder;
			cardRenderJob.position = screenPos;
			auto result = cardRenderSys->TryAdd(info, cardRenderJob);

			vke::SubTexture cardSubTexture = resourceSys->GetSubTexture(ResourceSystem::CardSubTextures::idle);

			if (job.cardIndex != SIZE_MAX)
			{
				const auto card = cardSys->GetCard(job.cardIndex);
				cardSubTexture = card.art;

				jlb::String str{};
				str.AllocateFromNumber(dumpAllocator, card.cost);

				TextRenderJob textCostJob{};
				textCostJob.center = true;
				textCostJob.origin = screenPos;
				textCostJob.origin.y += cardRenderJob.scale.y * .5f;
				textCostJob.text = str;
				textCostJob.scale = vke::PIXEL_SIZE_ENTITY;
				textCostJob.padding = static_cast<int32_t>(textCostJob.scale) / -2;
				result = textRenderSys->TryAdd(info, textCostJob);

				TextBoxJob cardTextBox{};
				cardTextBox.text = card.text;
				result = textBoxSys->TryAdd(info, cardTextBox);
			}

			auto& animLerp = job.updateInfo.animLerp;
			animLerp += info.deltaTime * 0.001f * cardAnimSpeed / CARD_ANIM_LENGTH;
			animLerp = fmodf(animLerp, 1);
			vke::Animation cardAnim{};
			cardAnim.lerp = animLerp;
			cardAnim.width = CARD_ANIM_LENGTH;
			auto sub = cardAnim.Evaluate(cardSubTexture, 0);

			cardRenderJob.subTexture = sub;
			result = cardRenderSys->TryAdd(info, cardRenderJob);

			outputs.Add(dumpAllocator, job.updateInfo);
		}
	}
}
