#include "pch.h"
#include "Game.h"

#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/SceneSystem.h"
#include "Systems/TurnSystem.h"
#include "Systems/GameManager.h"
#include "VkEngine/Systems/UIRenderSystem.h"
#include "Systems/MouseSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/ResourceManager.h"
#include "Systems/TextRenderHandler.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game
{
	void DefineSystems(const jlb::SystemsInitializer<vke::EngineData>& initializer)
	{
		// Core engine systems.
		initializer.DefineSystem<ResourceManager>();
		initializer.DefineSystem<vke::SceneSystem>();
		initializer.DefineSystem<vke::EntityRenderSystem>();
		initializer.DefineSystem<vke::UIRenderSystem>();

		// Game systems.
		initializer.DefineSystem<TurnSystem>();
		initializer.DefineSystem<MovementSystem>();
		initializer.DefineSystem<TextRenderHandler>();
		initializer.DefineSystem<MouseSystem>();

		// Final system for threading reasons.
		initializer.DefineSystem<vke::ThreadPoolSystem>();

		// High level game manager.
		initializer.DefineSystem<GameManager>();
	}
}
