#pragma once
#include "BVH.h"
#include "VkEngine/Systems/TaskSystem.h"
#include <DistanceTree.h>

namespace game
{
	typedef jlb::BoundingVolumeHierarchy::Instance CollisionTask;

	class CollisionSystem final : public vke::TaskSystem<CollisionTask>
	{
	public:
		[[nodiscard]] size_t GetIntersections(
			const glm::vec2& position, const glm::vec2& scale,
			jlb::ArrayView<uint32_t> outArray);

		[[nodiscard]] size_t ReserveTile(const glm::ivec2& position);
		[[nodiscard]] size_t CheckIfTileIsReserved(const glm::ivec2& position);

	private:
		jlb::BoundingVolumeHierarchy _bvh{};
		jlb::DistanceTree _distanceTree{};

		void Allocate(const vke::EngineData& info) override;
		void Free(const vke::EngineData& info) override;
		void OnUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::NestedVector<CollisionTask>& tasks) override;
		[[nodiscard]] size_t DefineCapacity(const vke::EngineData& info) override;
		[[nodiscard]]size_t DefineNestedCapacity(const vke::EngineData& info) override;
		[[nodiscard]] bool AutoClearOnFrameEnd() override;
	};
}
