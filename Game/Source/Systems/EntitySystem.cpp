#include "pch.h"
#include "Systems/EntitySystem.h"
#include "Systems/TurnSystem.h"

namespace game
{
	void EntitySystem::EndFrame(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		JobSystem<Entity*>::EndFrame(info, systems);

		const auto turnSys = systems.Get<TurnSystem>();
		if (turnSys->GetIfEndTickEvent())
			ClearJobs();
	}

	bool EntitySystem::AutoClearOnFrameEnd()
	{
		return false;
	}
}
