#include "pch.h"
#include "Systems/EntitySystem.h"

namespace game
{
	size_t EntitySystem::DefineCapacity(const vke::EngineData& info)
	{
		return ENTITY_CAPACITY;
	}
}
