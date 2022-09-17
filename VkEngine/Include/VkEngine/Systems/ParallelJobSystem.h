#pragma once
#include "JobSystem.h"
#include "ThreadPoolSystem.h"

namespace jlb
{
	template <typename Self, typename Job>
	class ParallelJobSystem : public vke::JobSystem<Job>
	{
	protected:
		typedef void (*threadableFunc)(const vke::EngineData& info, Systems<vke::EngineData> systems, 
			const NestedVector<Job>& jobs, StackAllocator& dumpAllocator, Self* selfPtr);

		void Awake(const vke::EngineData& info, Systems<vke::EngineData> systems) override;
		void OnPreUpdate(const vke::EngineData& info, Systems<vke::EngineData> systems,
			const NestedVector<Job>& jobs) override;

		[[nodiscard]] virtual threadableFunc DefineThreadable() = 0;

	private:
		threadableFunc _func = nullptr;
	};

	template <typename Self, typename Job>
	void ParallelJobSystem<Self, Job>::Awake(const vke::EngineData& info, Systems<vke::EngineData> systems)
	{
		vke::JobSystem<Job>::Awake(info, systems);
		_func = DefineThreadable();
	}

	template <typename Self, typename Job>
	void ParallelJobSystem<Self, Job>::OnPreUpdate(const vke::EngineData& info,
		Systems<vke::EngineData> systems, const NestedVector<Job>& jobs)
	{
		vke::JobSystem<Job>::OnPreUpdate(info, systems, jobs);

		vke::ThreadPoolJob threadjob{};
		threadjob.func = [](const vke::EngineData& info, const Systems<vke::EngineData> systems, void* userPtr)
		{
			const auto self = static_cast<ParallelJobSystem<Self, Job>*>(userPtr);

			const auto& jobs = self->GetJobs();
			auto& output = self->GetOutputEditable();
			auto& dumpAllocator = *info.dumpAllocator;

			self->_func(info, systems, jobs, output, dumpAllocator, static_cast<Self*>(self));
		};
		threadjob.userPtr = this;
		
		const auto threadSys = systems.Get<vke::ThreadPoolSystem>();
		const auto result = threadSys->TryAdd(info, threadjob);
		assert(result != SIZE_MAX);
	}
}
