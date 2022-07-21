#include "pch.h"
#include "Systems/CollisionSystem.h"
#include <Systems/TurnSystem.h>
#include "Systems/TurnThreadPoolSystem.h"

namespace game
{
	size_t CollisionSystem::GetIntersections(
		const glm::vec2& position, const glm::vec2& scale,
		const jlb::ArrayView<uint32_t> outArray)
	{
		return _bvh.GetIntersections(position, scale, GetTasks().GetRoot(), outArray);
	}

	size_t CollisionSystem::ReserveTile(const glm::ivec2& position)
	{
		return _distanceTree.Add(position);
	}

	size_t CollisionSystem::CheckIfTileIsReserved(const glm::ivec2& position)
	{
		size_t out = SIZE_MAX;
		const size_t _ = _distanceTree.GetInstancesInRange(position, .2f, out);
		return out;
	}

	void CollisionSystem::Allocate(const vke::EngineData& info)
	{
		TaskSystem<CollisionTask>::Allocate(info);
		_bvh.Allocate(*info.allocator, GetLength());
		_distanceTree.Allocate(*info.allocator, GetLength());
	}

	void CollisionSystem::Free(const vke::EngineData& info)
	{
		_distanceTree.Free(*info.allocator);
		_bvh.Free(*info.allocator);
		TaskSystem<CollisionTask>::Free(info);
	}

	size_t CollisionSystem::DefineNestedCapacity(const vke::EngineData& info)
	{
		return 0;
	}

	void CollisionSystem::OnUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
	    const jlb::NestedVector<CollisionTask>& tasks)
	{
		TaskSystem<CollisionTask>::OnUpdate(info, systems, tasks);
		const auto turnSys = systems.GetSystem<TurnSystem>();

		// If tick event, clear tasks.
		const bool isTickEvent = turnSys->GetIfTickEvent();

		// If the previous frame was for adding tasks.
		if (isTickEvent)
		{
			ClearTasks();
			_distanceTree.Clear();

			const auto turnThreadSys = systems.GetSystem<TurnThreadPoolSystem>();
			TurnThreadPoolTask task{};
			task.userPtr = this;
			task.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
			{
				const auto sys = reinterpret_cast<CollisionSystem*>(userPtr);
				auto& tasks = sys->GetTasks();

				// Compile into collision distance tree.
				if (sys->GetCount() > 0)
					sys->_bvh.Build(tasks.GetRoot());
			};

			const auto result = turnThreadSys->TryAdd(info, task);
			assert(result != SIZE_MAX);
		}
	}

	size_t CollisionSystem::DefineCapacity(const vke::EngineData& info)
	{
		return ENTITY_CAPACITY;
	}

	bool CollisionSystem::AutoClearOnFrameEnd()
	{
		return false;
	}
}
