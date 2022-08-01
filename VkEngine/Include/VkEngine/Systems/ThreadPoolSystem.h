#pragma once
#include <mutex>
#include <thread>
#include "TaskSystemWithOutput.h"

namespace vke
{
#ifndef THREAD_POOL_SYSTEM_CAPACITY
#define THREAD_POOL_SYSTEM_CAPACITY 8
#endif

#ifndef THREAD_POOL_SYSTEM_NESTED_CAPACITY
#define THREAD_POOL_SYSTEM_NESTED_CAPACITY 8
#endif

	struct ThreadPoolTask final
	{
		void (*func)(const EngineData& info, jlb::Systems<EngineData> systems, void* userPtr);
		void* userPtr = nullptr;
	};

	class ThreadPoolSystem final : public TaskSystem<ThreadPoolTask>
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
		std::atomic<size_t> _tasksRemaining = 0;
		std::atomic<size_t> _tasksUnfinished = 0;
		std::mutex _getNextTaskMutex{};
		size_t _threadCount = 0;

		struct ThreadSharedInfo final
		{
			EngineData const* info;
			jlb::Systems<EngineData> systems;
		} _threadSharedInfo{};

		void Allocate(const EngineData& info) override;
		void Free(const EngineData& info) override;
		void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems, 
			const jlb::NestedVector<ThreadPoolTask>& tasks) override;
		void OnPostUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::NestedVector<ThreadPoolTask>& tasks) override;
		void Exit(const EngineData& info, jlb::Systems<EngineData> systems) override;

		[[nodiscard]] size_t DefineCapacity(const EngineData& info) override;
		size_t DefineNestedCapacity(const EngineData& info) override;
	};
}
