#include "pch.h"
#include "Scenes/DemoScene.h"
#include "Archetypes/DummyArchetype.h"
#include "Archetypes/PlayerArchetype.h"
#include "Systems/CollisionSystem.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Systems/TileRenderSystem.h"

namespace game::demo
{
	void DemoScene::Allocate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		GameScene::Allocate(info, systems);

		const auto collisionSys = systems.Get<CollisionSystem>();
		const auto entitySys = systems.Get<EntitySystem>();

		const auto entityArchetypes = GetEntityArchetypes();
		auto& sceneAllocator = GetAllocator();

		auto& players = entityArchetypes.Get<PlayerArchetype>()->GetEntities();
		auto& pickups = entityArchetypes.Get<PickupArchetype>()->GetEntities();
		auto& dummies = entityArchetypes.Get<DummyArchetype>()->GetEntities();

		{
			auto& player = players.Add(sceneAllocator);
			auto& inventory = player.data.character.inventory;
			inventory.src = player.inventorySrc;
			auto& fireball = inventory.Insert(0);
			auto& bash = inventory.Insert(2);
		}

		{
			Pickup pickup{};
			pickup.data.pickup.cardId = 1;
			pickup.transform.position = glm::vec2{ 2, -1 };
			pickups.Add(sceneAllocator, pickup);
		}

		constexpr int32_t dummyCount = 6;
		for (int32_t i = 0; i < dummyCount; ++i)
			for (int32_t j = 0; j < dummyCount; ++j)
			{
				const auto pos = glm::vec2(i - dummyCount / 2 - dummyCount, j - dummyCount / 2 - dummyCount);

				DummyEntity entity{};
				entity.transform.position = pos;
				dummies.Add(sceneAllocator, entity);
			}

		for (auto& entity : players)
		{
			entitySys->CreateEntity(entity);
			const glm::ivec2 toRounded = jlb::math::RoundNearest(entity.transform.position);
			collisionSys->ReserveTilesNextTurn(toRounded);
		}

		for (auto& entity : pickups)
		{
			entitySys->CreateEntity(entity);
			const glm::ivec2 toRounded = jlb::math::RoundNearest(entity.transform.position);
			collisionSys->ReserveTilesNextTurn(toRounded);
		}

		for (auto& entity : dummies)
		{
			entitySys->CreateEntity(entity);
			const glm::ivec2 toRounded = jlb::math::RoundNearest(entity.transform.position);
			collisionSys->ReserveTilesNextTurn(toRounded);
		}
	}

	void DemoScene::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		GameScene::PreUpdate(info, systems);

		const auto collisionSys = systems.Get<CollisionSystem>();
		const auto tileSys = systems.Get<vke::TileRenderSystem>();
		const auto turnSys = systems.Get<TurnSystem>();

		// Tile test.
		vke::TileRenderTask tileTask{};
		tileTask.shape = glm::ivec2(3, 5);
		tileTask.position = glm::ivec2(3, 5);
		auto result = tileSys->TryAdd(info, tileTask);

		if(turnSys->GetIfBeginTickEvent())
		{
			CollisionTask collisionTask{};
			auto& bounds = collisionTask.bounds;
			bounds.lBot = glm::ivec2(2, 3);
			bounds.rTop = bounds.lBot + glm::ivec2(2, 4);
			bounds.layers = 0b11;
			result = collisionSys->TryAdd(collisionTask);
			assert(result != SIZE_MAX);
		}
	}

	void DemoScene::DefineSystems(jlb::SystemsInitializer<EntityArchetypeInfo> initializer)
	{
		initializer.DefineSystem<PlayerArchetype>();
		initializer.DefineSystem<PickupArchetype>();
		initializer.DefineSystem<DummyArchetype>();
	}
}
