#include "pch.h"
#include "Systems/CollisionSystem.h"
#include <Systems/TurnSystem.h>
#include "Systems/TurnThreadPoolSystem.h"

namespace game
{
	size_t CollisionSystem::TryAdd(const CollisionTask& task)
	{
		auto& previous = _collisionFrames.GetPrevious();
		previous.tasks.Add(task);
		return previous.tasks.GetCount() - 1;
	}

	size_t CollisionSystem::GetIntersections(
		const glm::vec2& position,  glm::vec2 scale,
		const jlb::ArrayView<uint32_t> outArray)
	{
		scale += FLT_EPSILON;
		auto& current = _collisionFrames.GetPrevious();
		return current.bvh.GetIntersections(glm::ivec2(position), current.tasks, outArray);
	}

	size_t CollisionSystem::ReserveTiles(const jlb::Bounds& bounds)
	{
		auto& current = _collisionFrames.GetCurrent();
		return current.distanceTree.Add(bounds);
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
			collisionFrame.tasks.Allocate(*info.allocator, ENTITY_CAPACITY);
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
			collisionFrame.tasks.Free(*info.allocator);
		}

		vke::GameSystem::Free(info);
	}

	void CollisionSystem::PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems)
	{
		System<vke::EngineData>::PreUpdate(info, systems);
		const auto turnSys = systems.GetSystem<TurnSystem>();

		// If the previous frame was for adding tasks.
		if (turnSys->GetIfTickEvent())
		{
			_collisionFrames.Swap();
			auto& previous = _collisionFrames.GetPrevious();
			previous.tasks.SetCount(0);
			previous.distanceTree.Clear();

			const auto turnThreadSys = systems.GetSystem<TurnThreadPoolSystem>();
			TurnThreadPoolTask task{};
			task.userPtr = this;
			task.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
			{
				const auto sys = reinterpret_cast<CollisionSystem*>(userPtr);
				auto& previous = sys->_collisionFrames.GetPrevious();

				// Compile into collision distance tree.
				const auto& tasks = previous.tasks;
				if (tasks.GetCount() > 0)
					previous.bvh.Build(tasks);
			};

			const auto result = turnThreadSys->TryAdd(info, task);
			assert(result != SIZE_MAX);
		}
	}
}
