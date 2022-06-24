#include "pch.h"
#include "Game.h"
#include "VkEngine/EntityRenderSystem.h"

namespace game
{
	void DefineSystems(const jlb::SystemsInitializer<vke::EngineData>& initializer)
	{
		initializer.DefineSystem<vke::EntityRenderSystem>();
	}
}
