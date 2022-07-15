#pragma once
#include "StringView.h"
#include "VkEngine/Systems/TaskSystem.h"

namespace game
{
	struct TextRenderTask final
	{
		// Text to render. Can include lowercase characters, spaces and numbers.
		jlb::StringView text{};
		// Origin position for the text.
		glm::vec2 origin;
		// Overrides the length of the string.
		size_t lengthOverride = SIZE_MAX;
		// Ignores origin value to place it after the text of target task.
		size_t appendIndex = SIZE_MAX;
		// Space between characters. Can be negative.
		int32_t padding = -2;
	};

	/*
	Forwards created tasks to the UIRenderSystem as UIRenderTasks.
	*/
	class TextRenderHandler final : public vke::TaskSystem<TextRenderTask>
	{
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::Vector<TextRenderTask>& tasks) override;
		[[nodiscard]] size_t DefineMinimalUsage(const vke::EngineData& info) override;
	};
}
