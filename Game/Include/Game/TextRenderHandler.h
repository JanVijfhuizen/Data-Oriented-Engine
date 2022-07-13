#pragma once
#include "StringView.h"
#include "VkEngine/Systems/TaskSystem.h"

namespace game
{
	struct TextRenderTask final
	{
		jlb::StringView text{};
		glm::vec2 origin;
		size_t lengthOverride = SIZE_MAX;
	};

	class TextRenderHandler final : public vke::TaskSystem<TextRenderTask>
	{
		void OnUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::Vector<TextRenderTask>& tasks) override;
		[[nodiscard]] size_t DefineMinimalUsage(const vke::EngineData& info) override;
	};
}
