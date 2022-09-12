#pragma once
#include "VkEngine/Systems/JobSystemWithOutput.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game
{
	template <typename Self, typename Job, typename Output>
	class ParallelJobSystemWithOutput : public vke::JobSystemWithOutput<Job, Output>
	{
	protected:
		typedef void (*threadableFunc)(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, 
			const jlb::NestedVector<Job>& jobs, jlb::NestedVector<Output>& output, jlb::StackAllocator& dumpAllocator, Self* selfPtr);

		void Awake(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::NestedVector<Job>& jobs) override;

		[[nodiscard]] virtual threadableFunc DefineThreadable() = 0;

	private:
		threadableFunc _func = nullptr;
	};

	template <typename Self, typename Input, typename Output>
	void ParallelJobSystemWithOutput<Self, Input, Output>::Awake(const vke::EngineData& info,
		const jlb::Systems<vke::EngineData> systems)
	{
		vke::JobSystemWithOutput<Input, Output>::Awake(info, systems);
		_func = DefineThreadable();
	}

	template <typename Self, typename Input, typename Output>
	void ParallelJobSystemWithOutput<Self, Input, Output>::OnPreUpdate(const vke::EngineData& info,
		const jlb::Systems<vke::EngineData> systems, const jlb::NestedVector<Input>& jobs)
	{
		vke::JobSystemWithOutput<Input, Output>::OnPreUpdate(info, systems, jobs);

		vke::ThreadPoolJob threadjob{};
		threadjob.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
		{
			const auto self = static_cast<ParallelJobSystemWithOutput<Self, Input, Output>*>(userPtr);
			
			const auto& jobs = self->GetJobs();
			auto& output = self->GetOutputEditable();
			auto& dumpAllocator = *info.dumpAllocator;

			self->_func(info, systems, jobs, output, dumpAllocator, static_cast<Self*>(self));
		};
		threadjob.userPtr = this;

		auto& jobsOutput = vke::JobSystemWithOutput<Input, Output>::GetOutputEditable();
		auto& dumpAllocator = *info.dumpAllocator;
		jobsOutput.PreAllocateNested(dumpAllocator, vke::JobSystemWithOutput<Input, Output>::GetCount());

		const auto threadSys = systems.Get<vke::ThreadPoolSystem>();
		const auto result = threadSys->TryAdd(info, threadjob);
		assert(result != SIZE_MAX);
	}
}
