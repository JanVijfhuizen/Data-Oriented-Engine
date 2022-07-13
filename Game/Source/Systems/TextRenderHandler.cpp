#include "pch.h"
#include "TextRenderHandler.h"
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
		const auto subTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::alphabet);
		const float chunkSize = vke::texture::GetChunkSize(subTexture, 26);

		for (const auto& task : tasks)
		{
			const size_t length = task.lengthOverride == SIZE_MAX ? task.text.GetLength() : task.lengthOverride;
			assert(task.lengthOverride == SIZE_MAX ? true : task.lengthOverride <= task.text.GetLength());

			glm::vec2 origin = task.origin;

			for (size_t i = 0; i < length; ++i)
			{
				origin.x += fontSize;

				const size_t position = task.text[i] - 'a';

				vke::SubTexture charSubTexture = subTexture;
				charSubTexture.lTop.x = chunkSize * position;
				charSubTexture.rBot.x = charSubTexture.lTop.x + chunkSize;

				vke::UIRenderTask uiRenderTask{};
				uiRenderTask.transform.position = origin;
				uiRenderTask.transform.scale = fontSize;
				uiRenderTask.subTexture = charSubTexture;

				const auto result = uiSys->TryAdd(uiRenderTask);
				assert(result);
			}
		}
	}

	size_t TextRenderHandler::DefineMinimalUsage(const vke::EngineData& info)
	{
		return 48;
	}
}
