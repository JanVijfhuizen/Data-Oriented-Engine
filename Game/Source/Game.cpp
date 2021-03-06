#include "pch.h"
#include "Game.h"
#include "Systems/CameraSystem.h"
#include "Systems/CollisionSystem.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/SceneSystem.h"
#include "Systems/TurnSystem.h"
#include "Systems/GameManager.h"
#include "VkEngine/Systems/UIRenderSystem.h"
#include "Systems/MouseSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/ResourceManager.h"
#include "Systems/TextRenderHandler.h"
#include "Systems/TurnThreadPoolSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"
#include "VkEngine/Systems/TileRenderSystem.h"

namespace game
{
	void DefineSystems(const jlb::SystemsInitializer<vke::EngineData>& initializer)
	{
		// Core engine pre update.
		initializer.DefineSystem<vke::ThreadPoolSystem>();
		initializer.DefineSystem<ResourceManager>();

		// Core game systems.
		initializer.DefineSystem<TurnSystem>();
		initializer.DefineSystem<TurnThreadPoolSystem>();
		
		// Game systems.
		initializer.DefineSystem<CollisionSystem>();
		initializer.DefineSystem<vke::SceneSystem>();
		initializer.DefineSystem<MovementSystem>();
		initializer.DefineSystem<TextRenderHandler>();
		initializer.DefineSystem<MouseSystem>();
		initializer.DefineSystem<CameraSystem>();

		// Core engine Post update.
		initializer.DefineSystem<vke::TileRenderSystem>();
		initializer.DefineSystem<vke::EntityRenderSystem>();
		initializer.DefineSystem<vke::UIRenderSystem>();

		// High level game manager.
		initializer.DefineSystem<GameManager>();
	}
}
