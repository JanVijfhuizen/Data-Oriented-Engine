#pragma once
#include "BVH.h"
#include "VkEngine/Systems/TaskSystem.h"
#include <DistanceTree.h>
#include "SwapChain.h"

namespace game
{
	typedef jlb::Bounds CollisionTask;

	class CollisionSystem final : public vke::GameSystem
	{
	public:
		[[nodiscard]] size_t TryAdd(const CollisionTask& task);

		[[nodiscard]] size_t GetIntersections(
			const glm::vec2& position, glm::vec2 scale,
			jlb::ArrayView<uint32_t> outArray);

		[[nodiscard]] size_t ReserveTiles(const jlb::Bounds& bounds);
		[[nodiscard]] size_t CheckIfTilesAreReserved(const jlb::Bounds& bounds);

	private:
		struct CollisionFrame final
		{
			jlb::BoundingVolumeHierarchy bvh{};
			jlb::DistanceTree distanceTree{};
			jlb::Vector<CollisionTask> tasks{};
		};

		jlb::SwapChain<CollisionFrame, 2> _collisionFrames{};

		void Allocate(const vke::EngineData& info) override;
		void Free(const vke::EngineData& info) override;
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
	};
}
