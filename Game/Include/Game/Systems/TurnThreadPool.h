#pragma once
#include <atomic>
#include <thread>

#include "VkEngine/Systems/TaskSystem.h"

namespace vke
{
	class ThreadPoolSystem;
}

namespace game
{
	struct TurnThreadPoolTask final
	{
		void (*func)(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, void* userPtr);
		void* userPtr = nullptr;
	};

	class TurnThreadPool final : public vke::TaskSystem<TurnThreadPoolTask>
	{
		// Pauses and continues managed threads based on how busy the ThreadPoolSystem is.
		struct ManagingThread final
		{
			void operator()(TurnThreadPool* sys, vke::ThreadPoolSystem* threadPoolSys) const;
		};

		bool _takesTasks = false;
		jlb::Allocation<std::thread> _managingThread{};
		jlb::Allocation<std::thread> _threads{};
		std::atomic<bool> _stopThreads = false;
		std::atomic<size_t> _tasksRemaining = 0;
		size_t _threadCount = 0;

		void Allocate(const vke::EngineData& info) override;
		void Free(const vke::EngineData& info) override;
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, const jlb::NestedVector<TurnThreadPoolTask>& tasks) override;
		void OnPostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, const jlb::NestedVector<TurnThreadPoolTask>& tasks) override;
		void Exit(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems) override;
		[[nodiscard]] bool AutoClearOnFrameEnd() override;
		bool ValidateOnTryAdd(const TurnThreadPoolTask& task) override;
	};
}
