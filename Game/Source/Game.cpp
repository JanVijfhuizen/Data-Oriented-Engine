#include "pch.h"
#include "Game.h"

#include "TextRenderHandler.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/SceneSystem.h"
#include "Systems/TurnSystem.h"
#include "Systems/GameState.h"
#include "VkEngine/Systems/UIRenderSystem.h"
#include "Systems/MouseSystem.h"
#include "Systems/ResourceManager.h"

namespace game
{
	void DefineSystems(const jlb::SystemsInitializer<vke::EngineData>& initializer)
	{
		// Core dependent systems.
		initializer.DefineSystem<TextRenderHandler>();

		// Core engine systems.
		initializer.DefineSystem<ResourceManager>();
		initializer.DefineSystem<vke::SceneSystem>();
		initializer.DefineSystem<vke::EntityRenderSystem>();
		initializer.DefineSystem<vke::UIRenderSystem>();

		// Game systems.
		initializer.DefineSystem<TurnSystem>();
		initializer.DefineSystem<MouseSystem>();

		// High level game manager.
		initializer.DefineSystem<GameState>();
	}
}
