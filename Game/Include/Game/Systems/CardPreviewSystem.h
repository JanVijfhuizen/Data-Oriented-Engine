#pragma once
#include "VkEngine/Systems/TaskSystemWithOutput.h"

namespace game
{
	struct CardPreviewTaskUpdateInfo final
	{
		float animLerp = 0;
	};

	struct CardPreviewTask final
	{
		size_t cardIndex = SIZE_MAX;
		glm::vec2 origin{};
		CardPreviewTaskUpdateInfo updateInfo{};
	};

	class CardPreviewSystem final : public vke::TaskSystemWithOutput<CardPreviewTask, CardPreviewTaskUpdateInfo>
	{
	public:
		float cardAnimSpeed = 5;

		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, const jlb::NestedVector<CardPreviewTask>& tasks) override;
	};
}

