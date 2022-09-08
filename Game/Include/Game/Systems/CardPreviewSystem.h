#pragma once
#include "VkEngine/Systems/JobSystemWithOutput.h"

namespace game
{
	struct CardPreviewJobUpdateInfo final
	{
		float animLerp = 0;
	};

	struct CardPreviewJob final
	{
		size_t cardIndex = SIZE_MAX;
		glm::vec2 origin{};
		CardPreviewJobUpdateInfo updateInfo{};
	};

	class CardPreviewSystem final : public vke::JobSystemWithOutput<CardPreviewJob, CardPreviewJobUpdateInfo>
	{
	public:
		float cardAnimSpeed = 5;

		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, const jlb::NestedVector<CardPreviewJob>& tasks) override;
	};
}

