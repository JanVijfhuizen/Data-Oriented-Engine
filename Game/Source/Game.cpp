#include "pch.h"
#include "Game.h"
#include "DemoGame.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/SceneSystem.h"

namespace game
{
	void DefineSystems(const jlb::SystemsInitializer<vke::EngineData>& initializer)
	{
		initializer.DefineSystem<DemoGame>();
		initializer.DefineSystem<vke::SceneSystem>();
		initializer.DefineSystem<vke::EntityRenderSystem>();
	}
}
