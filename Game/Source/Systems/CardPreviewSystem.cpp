#include "pch.h"
#include "Systems/CardPreviewSystem.h"
#include "JlbString.h"
#include "Systems/CardRenderSystem.h"
#include "Systems/CardSystem.h"
#include "Systems/ResourceManager.h"
#include "Systems/TextBoxSystem.h"
#include "Systems/TextRenderHandler.h"
#include "VkEngine/Graphics/Animation.h"
#include "VkEngine/Systems/EntityRenderSystem.h"

namespace game
{
	void CardPreviewSystem::OnPreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<CardPreviewTask>& tasks)
	{
		TaskSystemWithOutput<CardPreviewTask, CardPreviewTaskUpdateInfo>::OnPreUpdate(info, systems, tasks);

		const auto cardSys = systems.Get<CardSystem>();
		const auto cardRenderSys = systems.Get<CardRenderSystem>();
		const auto entityRenderSys = systems.Get<vke::EntityRenderSystem>();
		const auto resourceSys = systems.Get<ResourceManager>();
		const auto textBoxSys = systems.Get<TextBoxSystem>();
		const auto textRenderSys = systems.Get<TextRenderHandler>();

		auto& outputs = GetOutputEditable();

		for (auto& task : tasks)
		{
			auto& dumpAllocator = *info.dumpAllocator;

			const auto cardBorder = resourceSys->GetSubTexture(ResourceManager::CardSubTextures::border);
			const auto& pixelSize = cardRenderSys->camera.pixelSize;

			const auto worldPos = task.origin - entityRenderSys->camera.position;
			const auto screenPos = vke::UIRenderSystem::WorldToScreenPos(worldPos, cardRenderSys->camera, info.swapChainData->resolution);

			vke::UIRenderTask cardRenderTask{};
			cardRenderTask.scale = pixelSize * glm::vec2(static_cast<float>(vke::PIXEL_SIZE_ENTITY * 4));
			cardRenderTask.subTexture = cardBorder;
			cardRenderTask.position = screenPos;
			auto result = cardRenderSys->TryAdd(info, cardRenderTask);

			vke::SubTexture cardSubTexture = resourceSys->GetSubTexture(ResourceManager::CardSubTextures::idle);

			if (task.cardIndex != SIZE_MAX)
			{
				const auto card = cardSys->GetCard(task.cardIndex);
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

				TextBoxTask cardTextBox{};
				cardTextBox.text = card.text;
				result = textBoxSys->TryAdd(info, cardTextBox);
			}

			auto& animLerp = task.updateInfo.animLerp;
			animLerp += info.deltaTime * 0.001f * cardAnimSpeed / CARD_ANIM_LENGTH;
			animLerp = fmodf(animLerp, 1);
			vke::Animation cardAnim{};
			cardAnim.lerp = animLerp;
			cardAnim.width = CARD_ANIM_LENGTH;
			auto sub = cardAnim.Evaluate(cardSubTexture, 0);

			cardRenderTask.subTexture = sub;
			result = cardRenderSys->TryAdd(info, cardRenderTask);

			outputs.Add(dumpAllocator, task.updateInfo);
		}
	}
}
