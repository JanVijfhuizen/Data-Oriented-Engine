#include "pch.h"
#include "Systems/CollisionSystem.h"
#include <Systems/TurnSystem.h>
#include "Systems/TurnThreadPoolSystem.h"

namespace game
{
	size_t CollisionSystem::TryAdd(const CollisionJob& job)
	{
		auto& previous = _collisionFrames.GetPrevious();
		previous.jobs.Add(job);
		return previous.jobs.GetCount() - 1;
	}

	size_t CollisionSystem::GetIntersections(const jlb::Bounds& bounds, const jlb::ArrayView<uint32_t> outArray)
	{
		auto& current = _collisionFrames.GetCurrent();
		return current.bvh.GetIntersections(bounds, current.jobs, outArray);
	}

	size_t CollisionSystem::ReserveTilesThisTurn(const jlb::Bounds& bounds)
	{
		auto& current = _collisionFrames.GetCurrent();
		return current.distanceTree.Add(bounds);
	}

	size_t CollisionSystem::ReserveTilesNextTurn(const jlb::Bounds& bounds)
	{
		auto& previous = _collisionFrames.GetPrevious();
		return previous.distanceTree.Add(bounds);
	}

	size_t CollisionSystem::CheckIfTilesAreReserved(const jlb::Bounds& bounds)
	{
		auto& current = _collisionFrames.GetCurrent();
		uint32_t out = SIZE_MAX;
		const uint32_t count = current.distanceTree.GetInstancesInRange(bounds, out);
		return count == 0 ? SIZE_MAX : out;
	}

	void CollisionSystem::Allocate(const vke::EngineData& info)
	{
		vke::GameSystem::Allocate(info);
		for (auto& collisionFrame : _collisionFrames)
		{
			collisionFrame.jobs.Allocate(*info.allocator, ENTITY_CAPACITY);
			collisionFrame.bvh.Allocate(*info.allocator, ENTITY_CAPACITY);
			collisionFrame.distanceTree.Allocate(*info.allocator, ENTITY_CAPACITY);
		}
	}

	void CollisionSystem::Free(const vke::EngineData& info)
	{
		for (int32_t i = 1; i >= 0; --i)
		{
			auto& collisionFrame = _collisionFrames[i];
			collisionFrame.distanceTree.Free(*info.allocator);
			collisionFrame.bvh.Free(*info.allocator);
			collisionFrame.jobs.Free(*info.allocator);
		}

		vke::GameSystem::Free(info);
	}

	void CollisionSystem::PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems)
	{
		System<vke::EngineData>::PreUpdate(info, systems);
		const auto turnSys = systems.Get<TurnSystem>();

		// If the previous frame was for adding tasks.
		if (turnSys->GetIfBeginTickEvent())
		{
			_collisionFrames.Swap();
			auto& previous = _collisionFrames.GetPrevious();
			previous.jobs.SetCount(0);
			previous.distanceTree.Clear();
			
			TurnThreadPoolJob task{};
			task.userPtr = this;
			task.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
			{
				const auto sys = reinterpret_cast<CollisionSystem*>(userPtr);
				auto& previous = sys->_collisionFrames.GetPrevious();

				// Compile into collision distance tree.
				const auto& tasks = previous.jobs;
				if (tasks.GetCount() > 0)
					previous.bvh.Build(tasks);
			};

			const auto turnThreadSys = systems.Get<TurnThreadPoolSystem>();
			const auto result = turnThreadSys->TryAdd(info, task);
			assert(result != SIZE_MAX);
		}
	}
}
