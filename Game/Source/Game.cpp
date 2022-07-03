#include "pch.h"
#include "Game.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/SceneSystem.h"
#include "Systems/TurnSystem.h"
#include "Systems/GameState.h"

namespace game
{
	void DefineSystems(const jlb::SystemsInitializer<vke::EngineData>& initializer)
	{
		initializer.DefineSystem<TurnSystem>();
		initializer.DefineSystem<vke::SceneSystem>();
		initializer.DefineSystem<vke::EntityRenderSystem>();
		initializer.DefineSystem<GameState>();
	}
}
