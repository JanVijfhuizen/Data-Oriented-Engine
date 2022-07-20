#pragma once
#include "VkEngine/Systems/TaskSystem.h"

namespace game
{
	struct TurnThreadPoolTask final
	{
		void (*func)(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, void* userPtr);
		void* userPtr = nullptr;
	};

	class TurnThreadPool final : public vke::TaskSystem<TurnThreadPoolTask>
	{
		bool _takesTasks = false;

		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, const jlb::NestedVector<TurnThreadPoolTask>& tasks) override;
		[[nodiscard]] bool AutoClearOnFrameEnd() override;
		bool ValidateOnTryAdd(const TurnThreadPoolTask& task) override;
	};
}
