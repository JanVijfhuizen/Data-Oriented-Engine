#include "pch.h"
#include "Systems/MouseSystem.h"

namespace game
{
	void MouseSystem::Allocate(const vke::EngineData& info)
	{
	}

	void MouseSystem::Free(const vke::EngineData& info)
	{
	}

	void MouseSystem::Update(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		const auto& mousePos = info.mousePos;
		
	}
}
