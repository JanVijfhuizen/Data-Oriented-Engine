#include "pch.h"
#include "Game.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game
{
	void DefineSystems(const jlb::SystemsInitializer<vke::EngineData>& initializer)
	{
		initializer.DefineSystem<vke::ThreadPoolSystem>();
	}
}
