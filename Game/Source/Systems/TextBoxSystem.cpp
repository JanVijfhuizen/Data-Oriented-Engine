#include "pch.h"
#include "Systems/TextBoxSystem.h"
#include "Systems/ResourceSystem.h"
#include "Systems/TextRenderHandler.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	void TextBoxSystem::OnPreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<TextBoxJob>& jobs)
	{
		JobSystem<TextBoxJob>::OnPreUpdate(info, systems, jobs);

		const auto resourceSys = systems.Get<ResourceSystem>();
		const auto textRenderSys = systems.Get<TextRenderHandler>();
		const auto uiRenderSys = systems.Get<vke::UIRenderSystem>();

		for (const auto& job : jobs)
		{
			TextRenderJob renderJob{};
			renderJob.center = job.center;
			renderJob.origin = job.screenOrigin;
			renderJob.text = job.text;
			renderJob.maxWidth = job.maxWidth;
			renderJob.scale = job.scale;
			renderJob.padding = static_cast<int32_t>(renderJob.scale) / -2;

			const auto& pixelSize = uiRenderSys->camera.pixelSize;
			const auto scale = pixelSize * static_cast<float>(renderJob.scale);
			const auto lineCount = renderJob.GetLineCount();
			const auto aspectFix = vke::UIRenderSystem::GetAspectFix(info.swapChainData->resolution);

			vke::UIRenderJob backgroundRenderTask{};
			backgroundRenderTask.position = renderJob.origin;
			backgroundRenderTask.scale.x = aspectFix * (scale * static_cast<float>(renderJob.GetWidth()));
			backgroundRenderTask.scale.y = scale * static_cast<float>(lineCount);
			backgroundRenderTask.scale += job.borderSize * pixelSize;
			backgroundRenderTask.color = glm::vec4(0, 0, 0, 1);
			backgroundRenderTask.subTexture = resourceSys->GetSubTexture(ResourceSystem::UISubTextures::blank);
			backgroundRenderTask.position.y += scale * .5f * static_cast<float>(lineCount) - scale * .5f;
			auto result = uiRenderSys->TryAdd(info, backgroundRenderTask);
			result = textRenderSys->TryAdd(info, renderJob);
		}
	}
}
