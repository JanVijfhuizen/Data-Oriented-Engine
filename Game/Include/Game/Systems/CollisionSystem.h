#pragma once
#include "VkEngine/Systems/TaskSystem.h"

namespace game
{
	struct CollisionTask final
	{
		
	};

	class CollisionSystem final : public vke::TaskSystem<CollisionTask>
	{
		bool _mayAddTasks = false;

		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::NestedVector<CollisionTask>& tasks) override;
		bool ValidateOnTryAdd(const CollisionTask& task) override;
		[[nodiscard]] bool AutoClearOnFrameEnd() override;
	};
}
