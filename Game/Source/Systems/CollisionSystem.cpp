#include "pch.h"
#include "Systems/CollisionSystem.h"
#include <Systems/TurnSystem.h>
#include "Systems/TurnThreadPoolSystem.h"

namespace game
{
	size_t CollisionSystem::TryAdd(const CollisionTask& task)
	{
		auto& current = _collisionFrames.GetCurrent();
		current.tasks.Add(task);
		return current.tasks.GetCount() - 1;
	}

	size_t CollisionSystem::GetIntersections(
		const glm::vec2& position,  glm::vec2 scale,
		const jlb::ArrayView<uint32_t> outArray)
	{
		scale += FLT_EPSILON;
		auto& previous = _collisionFrames.GetPrevious();
		return previous.bvh.GetIntersections(position, scale, previous.tasks, outArray);
	}

	size_t CollisionSystem::ReserveTile(const glm::ivec2& position)
	{
		auto& current = _collisionFrames.GetCurrent();
		return current.distanceTree.Add(position);
	}

	size_t CollisionSystem::CheckIfTileIsReserved(const glm::ivec2& position)
	{
		auto& current = _collisionFrames.GetCurrent();
		size_t out = SIZE_MAX;
		const size_t _ = current.distanceTree.GetInstancesInRange(position, .2f, out);
		return out;
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

	void CollisionSystem::Update(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems)
	{
		System<vke::EngineData>::Update(info, systems);
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
				auto& frame = sys->_collisionFrames.GetPrevious();

				// Compile into collision distance tree.
				const auto& tasks = frame.tasks;
				if (tasks.GetCount() > 0)
					frame.bvh.Build(tasks);
			};

			const auto result = turnThreadSys->TryAdd(info, task);
			assert(result != SIZE_MAX);
		}
	}
}
