#pragma once
#include "VkEngine/Systems/TaskSystemWithOutput.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game
{
	template <typename Self, typename Input, typename Output>
	class ThreadedTaskSystemWithOutput : public vke::TaskSystemWithOutput<Input, Output>
	{
	protected:
		typedef void (*threadableFunc)(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, 
			const jlb::NestedVector<Input>& tasks, jlb::NestedVector<Output>& output, jlb::StackAllocator& dumpAllocator, Self* selfPtr);

		void Awake(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::NestedVector<Input>& tasks) override;

		[[nodiscard]] virtual threadableFunc DefineThreadable() = 0;

	private:
		threadableFunc _func = nullptr;
	};

	template <typename Self, typename Input, typename Output>
	void ThreadedTaskSystemWithOutput<Self, Input, Output>::Awake(const vke::EngineData& info,
		const jlb::Systems<vke::EngineData> systems)
	{
		vke::TaskSystemWithOutput<Input, Output>::Awake(info, systems);
		_func = DefineThreadable();
	}

	template <typename Self, typename Input, typename Output>
	void ThreadedTaskSystemWithOutput<Self, Input, Output>::OnPreUpdate(const vke::EngineData& info,
		const jlb::Systems<vke::EngineData> systems, const jlb::NestedVector<Input>& tasks)
	{
		vke::TaskSystemWithOutput<Input, Output>::OnPreUpdate(info, systems, tasks);

		vke::ThreadPoolTask threadTask{};
		threadTask.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
		{
			const auto self = static_cast<ThreadedTaskSystemWithOutput<Self, Input, Output>*>(userPtr);
			
			const auto& tasks = self->GetTasks();
			auto& output = self->GetOutputEditable();
			auto& dumpAllocator = *info.dumpAllocator;

			self->_func(info, systems, tasks, output, dumpAllocator, static_cast<Self*>(self));
		};
		threadTask.userPtr = this;

		auto& tasksOutput = vke::TaskSystemWithOutput<Input, Output>::GetOutputEditable();
		auto& dumpAllocator = *info.dumpAllocator;
		tasksOutput.PreAllocateNested(dumpAllocator, vke::TaskSystemWithOutput<Input, Output>::GetCount());

		const auto threadSys = systems.GetSystem<vke::ThreadPoolSystem>();
		const auto result = threadSys->TryAdd(info, threadTask);
		assert(result != SIZE_MAX);
	}
}
