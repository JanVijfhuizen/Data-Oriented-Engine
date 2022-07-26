#pragma once
#include "VkEngine/Components/Transform.h"
#include "VkEngine/Systems/TaskSystemWithOutput.h"

namespace game
{
	struct UIInteractionTask final
	{
		vke::Transform transform{};
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
