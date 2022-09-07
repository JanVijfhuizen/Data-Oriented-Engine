#include "pch.h"
#include "Systems/TextBoxSystem.h"
#include "Systems/ResourceManager.h"
#include "Systems/TextRenderHandler.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	void TextBoxSystem::OnPreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<TextBoxTask>& tasks)
	{
		TaskSystem<TextBoxTask>::OnPreUpdate(info, systems, tasks);

		const auto resourceSys = systems.Get<ResourceManager>();
		const auto textRenderSys = systems.Get<TextRenderHandler>();
		const auto uiRenderSys = systems.Get<vke::UIRenderSystem>();

		for (const auto& task : tasks)
		{
			TextRenderTask renderTask{};
			renderTask.center = task.center;
			renderTask.origin = task.screenOrigin;
			renderTask.text = task.text;
			renderTask.maxWidth = task.maxWidth;
			renderTask.scale = task.scale;
			renderTask.padding = static_cast<int32_t>(renderTask.scale) / -2;

			const auto& pixelSize = uiRenderSys->camera.pixelSize;
			const auto scale = pixelSize * static_cast<float>(renderTask.scale);
			const auto lineCount = renderTask.GetLineCount();
			const auto aspectFix = vke::UIRenderSystem::GetAspectFix(info.swapChainData->resolution);

			vke::UIRenderTask backgroundRenderTask{};
			backgroundRenderTask.position = renderTask.origin;
			backgroundRenderTask.scale.x = aspectFix * (scale * static_cast<float>(renderTask.GetWidth()));
			backgroundRenderTask.scale.y = scale * static_cast<float>(lineCount);
			backgroundRenderTask.scale += task.borderSize * pixelSize;
			backgroundRenderTask.color = glm::vec4(0, 0, 0, 1);
			backgroundRenderTask.subTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::blank);
			backgroundRenderTask.position.y += scale * .5f * static_cast<float>(lineCount) - scale * .5f;
			auto result = uiRenderSys->TryAdd(info, backgroundRenderTask);
			result = textRenderSys->TryAdd(info, renderTask);
		}
	}
}
