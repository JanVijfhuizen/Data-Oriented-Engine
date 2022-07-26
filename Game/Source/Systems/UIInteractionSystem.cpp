#include "pch.h"
#include "Systems/UIInteractionSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game
{
	void UIInteractionSystem::OnUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<UIInteractionTask>& uiInteractionTasks,
		jlb::NestedVector<UIInteractionTaskOutput>& taskOutputs)
	{
		TaskSystemWithOutput<UIInteractionTask, UIInteractionTaskOutput>::OnUpdate(
			info, systems, uiInteractionTasks, taskOutputs);
		if (!info.mouseAvailable)
			return;

		vke::ThreadPoolTask task{};
		task.userPtr = this;
		task.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
		{
			const auto self = reinterpret_cast<UIInteractionSystem*>(userPtr);
			auto& dumpAllocator = *info.dumpAllocator;
			const auto& mousePos = info.mousePos;

			const auto& tasks = self->GetTasks();
			auto& outputs = self->GetOutputEditable();

			const size_t count = tasks.GetCount();
			for (size_t i = 0; i < count; ++i)
			{
				UIInteractionTaskOutput output{};
				outputs.Add(dumpAllocator, output);
			}
		};

		const auto threadSys = systems.GetSystem<vke::ThreadPoolSystem>();
		const auto result = threadSys->TryAdd(info, task);
		assert(result != SIZE_MAX);
	}
}
