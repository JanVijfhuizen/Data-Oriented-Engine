#include "pch.h"
#include "Systems/UIInteractionSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game
{
	void UIInteractionSystem::OnPreUpdate(const vke::EngineData& info,
		const jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<UIInteractionTask>& uiInteractionTasks)
	{
		TaskSystem<UIInteractionTask>::OnUpdate(info, systems, uiInteractionTasks);

		_frameData.Swap();
		_frameData.GetPrevious() = {};

		if (!info.mouseAvailable)
			return;

		vke::ThreadPoolTask task{};
		task.userPtr = this;
		task.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
		{
			const auto self = reinterpret_cast<UIInteractionSystem*>(userPtr);
			const auto& mousePos = info.mousePos;

			const auto& tasks = self->GetTasks();

			const size_t count = tasks.GetCount();
			for (size_t i = 0; i < count; ++i)
			{
				const auto& task = tasks[i];
				if(task.bounds.Intersects(mousePos))
				{
					auto& frameData = self->_frameData.GetPrevious();
					frameData.index = i;
					break;
				}
			}
		};

		const auto threadSys = systems.GetSystem<vke::ThreadPoolSystem>();
		const auto result = threadSys->TryAdd(info, task);
		assert(result != SIZE_MAX);
	}
}
