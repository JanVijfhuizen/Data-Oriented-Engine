#pragma once
#include "BVH.h"
#include <DistanceTree.h>
#include "SwapChain.h"
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	typedef jlb::BoundingVolumeHierarchy::Instance CollisionJob;

	enum CollisionLayers
	{
		collisionLayerMain = 0b1,
		collisionLayerInteractable = 0b10
	};

	class CollisionSystem final : public vke::GameSystem
	{
	public:
		[[nodiscard]] size_t TryAdd(const CollisionJob& job);

		[[nodiscard]] size_t GetIntersections(const jlb::Bounds& bounds,
			jlb::ArrayView<uint32_t> outArray);

		size_t ReserveTilesThisTurn(const jlb::Bounds& bounds);
		size_t ReserveTilesNextTurn(const jlb::Bounds& bounds);
		[[nodiscard]] size_t CheckIfTilesAreReserved(const jlb::Bounds& bounds);

	private:
		struct CollisionFrame final
		{
			jlb::BoundingVolumeHierarchy bvh{};
			jlb::DistanceTree distanceTree{};
			jlb::Vector<CollisionJob> jobs{};
		};

		jlb::SwapChain<CollisionFrame, 2> _collisionFrames{};

		void Allocate(const vke::EngineData& info) override;
		void Free(const vke::EngineData& info) override;
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
	};
}
