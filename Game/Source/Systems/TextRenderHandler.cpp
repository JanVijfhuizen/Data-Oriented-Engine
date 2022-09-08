﻿#include "pch.h"
#include "Systems/TextRenderHandler.h"
#include "JlbMath.h"
#include "Systems/ResourceSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	size_t TextRenderJob::GetLineCount() const
	{
		return ceil(static_cast<float>(text.GetLength()) / maxWidth);
	}

	size_t TextRenderJob::GetWidth() const
	{
		return (maxWidth == SIZE_MAX ? text.GetLength() : jlb::math::Min(maxWidth, text.GetLength())) / 2;
	}

	void TextRenderHandler::OnPreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<TextRenderJob>& tasks)
	{
		JobSystem<TextRenderJob>::OnPreUpdate(info, systems, tasks);

		const auto uiSys = systems.Get<vke::UIRenderSystem>();
		const auto resourceSys = systems.Get<game::ResourceSystem>();

		const float pixelSize = uiSys->camera.pixelSize;

		const auto alphabetTexture = resourceSys->GetSubTexture(ResourceSystem::UISubTextures::alphabet);
		const float alphabetChunkSize = vke::texture::GetChunkSize(alphabetTexture, 26);

		const auto numbersTexture = resourceSys->GetSubTexture(ResourceSystem::UISubTextures::numbers);
		const float numbersChunkSize = vke::texture::GetChunkSize(numbersTexture, 10);

		const auto symbolsTexture = resourceSys->GetSubTexture(ResourceSystem::UISubTextures::symbols);
		const float symbolsChunkSize = vke::texture::GetChunkSize(symbolsTexture, 4);

		for (auto& task : tasks)
		{
			const float fontSize = pixelSize * task.scale;

			const size_t length = task.lengthOverride == SIZE_MAX ? task.text.GetLength() : task.lengthOverride;
			assert(task.lengthOverride == SIZE_MAX ? true : task.lengthOverride <= task.text.GetLength());

			const float paddedFontSize = fontSize + pixelSize * static_cast<float>(task.padding);
			glm::vec2 origin = task.origin;
			origin.x -= task.center ? paddedFontSize * .5f * jlb::math::Min<float>(task.maxWidth, length) - .5f * paddedFontSize : 0;

			// If the task is appending on another task.
			if (task.appendIndex != SIZE_MAX)
			{
				const auto& otherTask = tasks[task.appendIndex];
				const size_t otherLength = otherTask.lengthOverride == SIZE_MAX ? otherTask.text.GetLength() : otherTask.lengthOverride;
				const float additionalOffset = (fontSize + pixelSize * static_cast<float>(otherTask.padding)) * otherLength;
				origin = otherTask.origin;
				origin.x += additionalOffset;
				task.origin = origin;
			}
			origin.x -= paddedFontSize;

			glm::vec2 current = origin;
			size_t xRemaining = task.maxWidth;

			for (size_t i = 0; i < length; ++i)
			{
				current.x += paddedFontSize;

				const auto& c = task.text[i];

				--xRemaining;
				xRemaining = xRemaining == SIZE_MAX ? 0 : xRemaining;

				// If this is a space.
				if (c == ' ')
				{
					bool newLine = xRemaining == 0;
					
					if(!newLine)
					{
						size_t wordLength = 0;
						size_t j = i;
						while (j < length)
						{
							++j;
							const bool end = task.text[j] == ' ';
							j = end ? length : j;
							wordLength += !end;
						}

						newLine = wordLength > xRemaining;
					}

					if (newLine)
					{
						current.y += fontSize;
						current.x = origin.x;
						xRemaining = task.maxWidth;
					}
					
					continue;
				}

				const bool isSymbol = c < '0';
				const bool isInteger = !isSymbol && c < 'a';
				// Assert if it's a valid character.
				assert(isInteger ? c >= '0' && c <= '9' : isSymbol ? c >= ',' && c <= '/' : c >= 'a' && c <= 'z');

				const size_t position = static_cast<unsigned char>(c - (isInteger ? '0' : isSymbol ? ',' : 'a'));
				const float chunkSize = isInteger ? numbersChunkSize : isSymbol ? symbolsChunkSize : alphabetChunkSize;

				vke::SubTexture charSubTexture = isInteger ? numbersTexture : isSymbol ? symbolsTexture : alphabetTexture;
				charSubTexture.lTop.x += chunkSize * position;
				charSubTexture.rBot.x = charSubTexture.lTop.x + chunkSize;

				vke::UIRenderJob uiRenderTask{};
				uiRenderTask.position = current;
				uiRenderTask.scale = glm::vec2(fontSize);
				uiRenderTask.subTexture = charSubTexture;
				uiRenderTask.color = task.color;

				const auto result = uiSys->TryAdd(info, uiRenderTask);
				assert(result != SIZE_MAX);
			}
		}
	}

	size_t TextRenderHandler::DefineCapacity(const vke::EngineData& info)
	{
		return 48;
	}
}
