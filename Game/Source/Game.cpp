#include "pch.h"
#include "Game.h"

#include "Archetype.h"
#include "Systems/CameraSystem.h"
#include "Systems/CardPreviewSystem.h"
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
#include "Systems/PickupSystem.h"
#include "Systems/PlayerSystem.h"
#include "Systems/ResourceSystem.h"
#include "Systems/TextBoxSystem.h"
#include "Systems/TextRenderHandler.h"
#include "Systems/TurnThreadPoolSystem.h"
#include "Systems/UIInteractionSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"
#include "VkEngine/Systems/TileRenderSystem.h"

namespace game
{
	void DefineSystems(const jlb::SystemsInitializer<vke::EngineData>& initializer)
	{
		jlb::StackAllocator alloc{};
		alloc.Allocate({});
		jlb::Archetype archetype{};
		archetype.Allocate<int, float, bool>(alloc, 12);

		jlb::NestedVector<int> i;
		jlb::NestedVector<bool> b;
		archetype.GetView(i, b);

		// Core engine pre update.
		initializer.DefineSystem<ResourceSystem>();

		// Core game systems.
		initializer.DefineSystem<TurnSystem>();
		initializer.DefineSystem<vke::ThreadPoolSystem>();
		initializer.DefineSystem<TurnThreadPoolSystem>();
		
		// Game systems.
		initializer.DefineSystem<EntitySystem>();
		initializer.DefineSystem<CollisionSystem>();
		initializer.DefineSystem<vke::SceneSystem>();
		initializer.DefineSystem<MovementSystem>();
		initializer.DefineSystem<PlayerSystem>();
		initializer.DefineSystem<PickupSystem>();
		initializer.DefineSystem<CardSystem>();
		initializer.DefineSystem<MenuSystem>();
		initializer.DefineSystem<CardPreviewSystem>();
		initializer.DefineSystem<TextBoxSystem>();
		initializer.DefineSystem<TextRenderHandler>();
		initializer.DefineSystem<MouseSystem>();
		initializer.DefineSystem<CameraSystem>();
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
