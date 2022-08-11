#include "pch.h"
#include "Game.h"
#include "Systems/CameraSystem.h"
#include "Systems/CardRenderSystem.h"
#include "Systems/CardSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/EntitySystem.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/SceneSystem.h"
#include "Systems/TurnSystem.h"
#include "Systems/GameManager.h"
#include "Systems/MenuSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"
#include "Systems/MouseSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/PlayerSystem.h"
#include "Systems/ResourceManager.h"
#include "Systems/TextRenderHandler.h"
#include "Systems/TurnThreadPoolSystem.h"
#include "Systems/UIInteractionSystem.h"
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
		initializer.DefineSystem<EntitySystem>();
		initializer.DefineSystem<CollisionSystem>();
		initializer.DefineSystem<vke::SceneSystem>();
		initializer.DefineSystem<PlayerSystem>();
		initializer.DefineSystem<CardSystem>();
		initializer.DefineSystem<MovementSystem>();
		initializer.DefineSystem<TextRenderHandler>();
		initializer.DefineSystem<MouseSystem>();
		initializer.DefineSystem<CameraSystem>();
		initializer.DefineSystem<MenuSystem>();
		initializer.DefineSystem<UIInteractionSystem>();

		// Core engine Post update.
		initializer.DefineSystem<vke::TileRenderSystem>();
		initializer.DefineSystem<vke::EntityRenderSystem>();
		initializer.DefineSystem<CardRenderSystem>();
		initializer.DefineSystem<vke::UIRenderSystem>();

		// High level game manager.
		initializer.DefineSystem<GameManager>();
	}
}
