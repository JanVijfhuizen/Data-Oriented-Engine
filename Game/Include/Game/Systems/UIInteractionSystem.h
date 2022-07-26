#pragma once
#include "VkEngine/Graphics/SubTexture.h"
#include "VkEngine/Systems/TaskSystemWithOutput.h"

namespace game
{
	struct UIInteractionTask final
	{
		vke::SubTexture bounds{};
	};

	struct UIInteractionTaskOutput final
	{
		bool hovered = false;
	};

	class UIInteractionSystem final : public vke::TaskSystemWithOutput<UIInteractionTask, UIInteractionTaskOutput>
	{
		void OnUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::NestedVector<UIInteractionTask>&,
			jlb::NestedVector<UIInteractionTaskOutput>& taskOutputs) override;
	};
}
