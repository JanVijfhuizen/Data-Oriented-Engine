#include "pch.h"
#include "Systems/HandSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game
{
	void HandSystem::OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
		const jlb::NestedVector<HandComponent>& tasks)
	{
		TaskSystemWithOutput<HandComponent, HandComponent>::OnPreUpdate(info, systems, tasks);

		vke::ThreadPoolTask threadTask{};
		threadTask.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
		{
			const auto self = static_cast<HandSystem*>(userPtr);
			
			const auto& tasks = self->GetTasks();
			auto& tasksOutput = self->GetOutputEditable();
			auto& dumpAllocator = *info.dumpAllocator;

			for (auto& task : tasks)
			{
				tasksOutput.Add(dumpAllocator, task);
			}
		};
		threadTask.userPtr = this;

		auto& tasksOutput = GetOutputEditable();
		auto& dumpAllocator = *info.dumpAllocator;
		tasksOutput.PreAllocateNested(dumpAllocator, GetCount());

		const auto threadSys = systems.GetSystem<vke::ThreadPoolSystem>();
		const auto result = threadSys->TryAdd(info, threadTask);
		assert(result != SIZE_MAX);
	}
}
