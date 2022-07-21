#pragma once
#include "BVH.h"
#include "VkEngine/Systems/TaskSystem.h"

namespace game
{
	typedef jlb::BoundingVolumeHierarchy::Instance CollisionTask;

	class CollisionSystem final : public vke::TaskSystem<CollisionTask>
	{
		jlb::BoundingVolumeHierarchy _bvh{};

		void Allocate(const vke::EngineData& info) override;
		void Free(const vke::EngineData& info) override;
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::NestedVector<CollisionTask>& tasks) override;
		[[nodiscard]] size_t DefineCapacity(const vke::EngineData& info) override;
		[[nodiscard]]size_t DefineNestedCapacity(const vke::EngineData& info) override;
		[[nodiscard]] bool AutoClearOnFrameEnd() override;
	};
}
