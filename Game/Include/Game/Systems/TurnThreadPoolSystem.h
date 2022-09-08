#pragma once
#include <atomic>
#include <thread>
#include "VkEngine/Systems/JobSystem.h"

namespace vke
{
	class ThreadPoolSystem;
}

namespace game
{
	struct TurnThreadPoolJob final
	{
		void (*func)(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, void* userPtr);
		void* userPtr = nullptr;
	};

	class TurnThreadPoolSystem final : public vke::JobSystem<TurnThreadPoolJob>
	{
		// Pauses and continues managed threads based on how busy the ThreadPoolSystem is.
		struct ThreadObj final
		{
			void operator()(TurnThreadPoolSystem* sys) const;
		};

		bool _takesTasks = false;
		jlb::Allocation<std::thread> _thread{};
		std::atomic<bool> _stopThreads = false;
		std::atomic<size_t> _tasksRemaining = 0;
		std::atomic<size_t> _tasksUnfinished = 0;

		struct ThreadSharedInfo final
		{
			vke::EngineData const* info;
			jlb::Systems<vke::EngineData> systems;
		} _threadSharedInfo{};

		void Allocate(const vke::EngineData& info) override;
		void Free(const vke::EngineData& info) override;
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, const jlb::NestedVector<TurnThreadPoolJob>& tasks) override;
		void OnPostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, const jlb::NestedVector<TurnThreadPoolJob>& tasks) override;
		void Exit(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems) override;
		[[nodiscard]] bool AutoClearOnFrameEnd() override;
		[[nodiscard]] bool ValidateOnTryAdd(const TurnThreadPoolJob& task) override;
		[[nodiscard]] size_t DefineCapacity(const vke::EngineData& info) override;
		size_t DefineNestedCapacity(const vke::EngineData& info) override;
	};
}
