#include "pch.h"
#include "Systems/UIInteractionSystem.h"

namespace game
{
	void UIInteractionSystem::OnUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<UIInteractionTask>& uiInteractionTasks,
		jlb::NestedVector<UIInteractionTaskOutput>& taskOutputs)
	{
		TaskSystemWithOutput<UIInteractionTask, UIInteractionTaskOutput>::OnUpdate(
			info, systems, uiInteractionTasks, taskOutputs);

		for (const auto& task : uiInteractionTasks)
		{
			
		}
	}
}
