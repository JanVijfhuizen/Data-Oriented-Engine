#include "pch.h"
#include "Systems/CollisionSystem.h"
#include <Systems/TurnSystem.h>

#include "Systems/TurnThreadPoolSystem.h"

namespace game
{
	void CollisionSystem::Allocate(const vke::EngineData& info)
	{
		TaskSystem<CollisionTask>::Allocate(info);
		for (auto& bvh : _bvhs)
			bvh.Allocate(*info.allocator, GetLength());
	}

	void CollisionSystem::Free(const vke::EngineData& info)
	{
		for (int32_t i = _bvhs.GetLength() - 1; i >= 0; --i)
			_bvhs[i].Free(*info.allocator);

		TaskSystem<CollisionTask>::Free(info);
	}

	size_t CollisionSystem::DefineNestedCapacity(const vke::EngineData& info)
	{
		return 0;
	}

	void CollisionSystem::OnPreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
	    const jlb::NestedVector<CollisionTask>& tasks)
	{
		TaskSystem<CollisionTask>::OnPreUpdate(info, systems, tasks);
		const auto turnSys = systems.GetSystem<TurnSystem>();

		// If tick event, clear tasks.
		const bool isTickEvent = turnSys->GetIfTickEvent();

		// If the previous frame was for adding tasks.
		if (isTickEvent)
		{
			_bvhs.Swap();

			const auto turnThreadSys = systems.GetSystem<TurnThreadPoolSystem>();
			TurnThreadPoolTask task{};
			task.userPtr = this;
			task.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
			{
				const auto sys = reinterpret_cast<CollisionSystem*>(userPtr);
				auto& tasks = sys->GetTasks();

				// Compile into collision distance tree.
				if (sys->GetCount() > 0)
					sys->_bvhs.GetCurrent().Build(tasks.GetRoot());
				sys->ClearTasks();
			};

			const auto result = turnThreadSys->TryAdd(info, task);
			assert(result != SIZE_MAX);
		}
	}

	size_t CollisionSystem::DefineCapacity(const vke::EngineData& info)
	{
		return 1024;
	}

	bool CollisionSystem::AutoClearOnFrameEnd()
	{
		return false;
	}
}
