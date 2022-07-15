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
		// Core engine pre update.
		initializer.DefineSystem<vke::ThreadPoolSystem>();
		initializer.DefineSystem<ResourceManager>();
		initializer.DefineSystem<vke::SceneSystem>();

		// Game systems.
		initializer.DefineSystem<TurnSystem>();
		initializer.DefineSystem<MovementSystem>();
		initializer.DefineSystem<TextRenderHandler>();
		initializer.DefineSystem<MouseSystem>();

		// Core engine Post update.
		initializer.DefineSystem<vke::EntityRenderSystem>();
		initializer.DefineSystem<vke::UIRenderSystem>();

		// High level game manager.
		initializer.DefineSystem<GameManager>();
	}
}
