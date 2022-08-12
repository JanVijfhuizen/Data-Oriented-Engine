#include "pch.h"
#include "Systems/PlayerSystem.h"

namespace game
{
	void PlayerSystem::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		System<vke::EngineData>::PreUpdate(info, systems);
		pickupEntity = {};
	}
}
