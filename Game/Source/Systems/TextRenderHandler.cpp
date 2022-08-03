#include "pch.h"
#include "Systems/TextRenderHandler.h"
#include "Systems/ResourceManager.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	void TextRenderHandler::OnPreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<TextRenderTask>& tasks)
	{
		TaskSystem<TextRenderTask>::OnPreUpdate(info, systems, tasks);

		const auto uiSys = systems.GetSystem<vke::UIRenderSystem>();
		const auto resourceSys = systems.GetSystem<game::ResourceManager>();

		const float pixelSize = uiSys->camera.pixelSize;

		const auto alphabetTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::alphabet);
		const float alphabetChunkSize = vke::texture::GetChunkSize(alphabetTexture, 26);

		const auto numbersTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::numbers);
		const float numbersChunkSize = vke::texture::GetChunkSize(numbersTexture, 10);

		const auto symbolsTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::symbols);
		const float symbolsChunkSize = vke::texture::GetChunkSize(symbolsTexture, 2);

		for (auto& task : tasks)
		{
			const float fontSize = pixelSize * task.scale;

			const size_t length = task.lengthOverride == SIZE_MAX ? task.text.GetLength() : task.lengthOverride;
			assert(task.lengthOverride == SIZE_MAX ? true : task.lengthOverride <= task.text.GetLength());

			const float paddedFontSize = fontSize + pixelSize * static_cast<float>(task.padding);
			glm::vec2 origin = task.origin;
			origin.x -= task.center ? paddedFontSize * .5f * length - .5f * paddedFontSize : 0;

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

			for (size_t i = 0; i < length; ++i)
			{
				origin.x += paddedFontSize;

				const auto& c = task.text[i];

				// If this is a space.
				if (c == 32)
					continue;

				const bool isSymbol = c < '0';
				const bool isInteger = !isSymbol && c < 'a';
				// Assert if it's a valid character.
				assert(isInteger ? c >= '0' && c <= '9' : isSymbol ? c >= '.' && c <= '/' : c >= 'a' && c <= 'z');

				const size_t position = static_cast<unsigned char>(c - (isInteger ? '0' : isSymbol ? '.' : 'a'));
				const float chunkSize = isInteger ? numbersChunkSize : isSymbol ? symbolsChunkSize : alphabetChunkSize;

				vke::SubTexture charSubTexture = isInteger ? numbersTexture : isSymbol ? symbolsTexture : alphabetTexture;
				charSubTexture.lTop.x += chunkSize * position;
				charSubTexture.rBot.x = charSubTexture.lTop.x + chunkSize;

				vke::UIRenderTask uiRenderTask{};
				uiRenderTask.position = origin;
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
