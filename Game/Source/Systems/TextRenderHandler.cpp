﻿#include "pch.h"
#include "Systems/TextRenderHandler.h"
#include "Systems/ResourceManager.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	void TextRenderHandler::OnUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::Vector<TextRenderTask>& tasks)
	{
		const auto uiSys = systems.GetSystem<vke::UIRenderSystem>();
		const auto entitySys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto resourceSys = systems.GetSystem<game::ResourceManager>();

		const float fontSize = entitySys->camera.pixelSize * vke::PIXEL_SIZE_ENTITY;

		const auto alphabetTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::alphabet);
		const float alphabetChunkSize = vke::texture::GetChunkSize(alphabetTexture, 26);

		const auto numbersTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::numbers);
		const float numbersChunkSize = vke::texture::GetChunkSize(numbersTexture, 10);

		for (const auto& task : tasks)
		{
			const size_t length = task.lengthOverride == SIZE_MAX ? task.text.GetLength() : task.lengthOverride;
			assert(task.lengthOverride == SIZE_MAX ? true : task.lengthOverride <= task.text.GetLength());

			glm::vec2 origin = task.origin;

			// If the task is appending on another task.
			if (task.appendIndex != SIZE_MAX)
			{
				const auto& otherTask = tasks[task.appendIndex];
				const size_t otherLength = otherTask.lengthOverride == SIZE_MAX ? otherTask.text.GetLength() : otherTask.lengthOverride;
				const float additionalOffset = (fontSize + entitySys->camera.pixelSize * static_cast<float>(otherTask.padding)) * otherLength;
				origin = otherTask.origin;
				origin.x += additionalOffset;
			}

			const float paddedFontSize = fontSize + entitySys->camera.pixelSize * static_cast<float>(task.padding);
			origin.x -= paddedFontSize;

			for (size_t i = 0; i < length; ++i)
			{
				origin.x += paddedFontSize;

				const auto& c = task.text[i];

				// If this is a space.
				if (c == 32)
					continue;

				const bool isInteger = c < 'a';
				// Assert if it's a valid character.
				assert(isInteger ? c >= '0' && c <= '9' : c >= 'a' && c <= 'z');

				const size_t position = static_cast<unsigned char>(c - (isInteger ? '0' : 'a'));

				const float chunkSize = isInteger ? numbersChunkSize : alphabetChunkSize;

				vke::SubTexture charSubTexture = isInteger ? numbersTexture : alphabetTexture;
				charSubTexture.lTop.x = chunkSize * position;
				charSubTexture.rBot.x = charSubTexture.lTop.x + chunkSize;

				vke::UIRenderTask uiRenderTask{};
				uiRenderTask.transform.position = origin;
				uiRenderTask.transform.scale = fontSize;
				uiRenderTask.subTexture = charSubTexture;

				const auto result = uiSys->TryAdd(uiRenderTask);
				assert(result != SIZE_MAX);
			}
		}
	}

	size_t TextRenderHandler::DefineMinimalUsage(const vke::EngineData& info)
	{
		return 48;
	}
}