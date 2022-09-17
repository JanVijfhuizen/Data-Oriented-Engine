#pragma once
#include <mutex>
#include <thread>
#include "JobSystem.h"

namespace vke
{
#ifndef THREAD_POOL_SYSTEM_CAPACITY
#define THREAD_POOL_SYSTEM_CAPACITY 8
#endif

#ifndef THREAD_POOL_SYSTEM_NESTED_CAPACITY
#define THREAD_POOL_SYSTEM_NESTED_CAPACITY 8
#endif

	struct ThreadPoolJob final
	{
		void (*func)(const EngineData& info, jlb::Systems<EngineData> systems, void* userPtr);
		void* userPtr = nullptr;
	};

	class ThreadPoolSystem final : public JobSystem<ThreadPoolJob>
	{
		friend struct ThreadObj;

	public:
		[[nodiscard]] static size_t GetThreadCount();
		[[nodiscard]] size_t GetFreeThreadSlots() const;

	private:
		struct ThreadObj final
		{
			void operator()(ThreadPoolSystem* sys) const;
		};

		jlb::Allocation<std::thread> _threads{};
		std::atomic<bool> _stopThreads = false;
		std::atomic<size_t> _jobssRemaining = 0;
		std::atomic<size_t> _jobsUnfinished = 0;
		std::mutex _getNextJobsMutex{};
		size_t _threadCount = 0;

		struct ThreadSharedInfo final
		{
			EngineData const* info;
			jlb::Systems<EngineData> systems;
		} _threadSharedInfo{};

		void Allocate(const EngineData& info) override;
		void Free(const EngineData& info) override;
		void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems, 
			const jlb::NestedVector<ThreadPoolJob>& jobs) override;
		void OnPostUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::NestedVector<ThreadPoolJob>& jobs) override;
		void Exit(const EngineData& info, jlb::Systems<EngineData> systems) override;

		[[nodiscard]] size_t DefineCapacity(const EngineData& info) override;
		size_t DefineNestedCapacity(const EngineData& info) override;
	};
}
