#pragma once
#include <mutex>
#include <thread>
#include "GameSystem.h"
#include "TaskSystemWithOutput.h"

namespace vke
{
	struct ThreadPoolTask final
	{
		void (*func)(const EngineData& info, jlb::Systems<EngineData> systems);
	};

	class ThreadPoolSystem final : public TaskSystem<ThreadPoolTask>
	{
		friend struct ThreadObj;

		struct ThreadObj final
		{
			void operator()(ThreadPoolSystem* sys) const;
		};

		jlb::Array<std::thread> _threads{};
		std::atomic<bool> _stopThreads = false;
		std::atomic<size_t> _tasksRemaining = 0;
		std::mutex _getNextTaskMutex{};

		struct ThreadSharedInfo final
		{
			EngineData const* info;
			jlb::Systems<EngineData> systems;
		} _threadSharedInfo;

		void Allocate(const EngineData& info) override;
		void Free(const EngineData& info) override;
		void OnPreUpdate(const EngineData& info, jlb::Systems<EngineData> systems, 
			const jlb::Vector<ThreadPoolTask>& tasks) override;
		void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::Vector<ThreadPoolTask>& tasks) override;
		void Exit(const EngineData& info, jlb::Systems<EngineData> systems) override;

		[[nodiscard]] size_t DefineMinimalUsage(const EngineData& info) override;
	};
}
