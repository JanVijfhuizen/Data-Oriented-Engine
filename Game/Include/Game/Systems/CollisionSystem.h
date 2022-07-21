#pragma once
#include "BVH.h"
#include "SwapChain.h"
#include "VkEngine/Systems/TaskSystem.h"

namespace game
{
	typedef jlb::BoundingVolumeHierarchy::Instance CollisionTask;

	class CollisionSystem final : public vke::TaskSystem<CollisionTask>
	{
		jlb::SwapChain<jlb::BoundingVolumeHierarchy, 2> _bvhs{};
		bool _mayAddTasks = false;

		void Allocate(const vke::EngineData& info) override;
		void Free(const vke::EngineData& info) override;
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::NestedVector<CollisionTask>& tasks) override;
		bool ValidateOnTryAdd(const CollisionTask& task) override;
		[[nodiscard]] size_t DefineCapacity(const vke::EngineData& info) override;
		[[nodiscard]]size_t DefineNestedCapacity(const vke::EngineData& info) override;
		[[nodiscard]] bool AutoClearOnFrameEnd() override;
	};
}
