#include "pch.h"
#include "Archetypes/PlayerArchetype.h"

#include "Systems/MovementSystem.h"
#include "Systems/ResourceManager.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Systems/EntityRenderSystem.h"

namespace game
{
	void PlayerArchetype::PreUpdate(const vke::EngineData& info,
		const jlb::Systems<vke::EngineData> systems,
		const jlb::ArrayView<Player> entities)
	{
		Archetype<Player>::PreUpdate(info, systems, entities);

		const auto entityRenderSys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto movementSys = systems.GetSystem<MovementSystem>();
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto turnSys = systems.GetSystem<TurnSystem>();

		const auto subTexture = resourceSys->GetSubTexture(ResourceManager::EntitySubTextures::humanoid);
		jlb::StackArray<vke::SubTexture, 2> subTexturesDivided{};
		vke::texture::Subdivide(subTexture, 2, subTexturesDivided);

		vke::EntityRenderTask renderTask{};
		renderTask.subTexture = subTexturesDivided[0];

		MovementTask movementTask{};
		movementTask.duration = 4;

		const bool isTickEvent = turnSys->GetIfTickEvent();

		for (auto& entity : entities)
		{
			// Testing.
			if(isTickEvent && entity.movementComponent.remaining == 0)
			{
				auto randDir = glm::ivec2(rand() % 2 - 1);
				randDir.y = randDir.x == 0 ? rand() % 2 * 2 - 1 : 0;
				const auto finalDir = glm::vec2(randDir) * static_cast<float>(vke::PIXEL_SIZE_ENTITY);

				entity.movementComponent.from = entity.transform.position;
				entity.movementComponent.to = entity.movementComponent.from + finalDir;
				entity.movementComponent.remaining = movementTask.duration;
				entity.movementComponent.rotation = entity.transform.rotation;
			}

			renderTask.transform = entity.transform;
			renderTask.transform.scale *= entity.movementComponent.scaleMultiplier;

			const auto result = entityRenderSys->TryAdd(renderTask);
			assert(result != SIZE_MAX);

			movementTask.component = entity.movementComponent;
			entity.movementTaskId = movementSys->TryAdd(movementTask);
			assert(entity.movementTaskId != SIZE_MAX);
		}
	}

	void PlayerArchetype::PostUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::ArrayView<Player> entities)
	{
		Archetype<Player>::PostUpdate(info, systems, entities);

		const auto movementSys = systems.GetSystem<MovementSystem>();

		const auto& movementOutputs = movementSys->GetOutput();

		for (auto& entity : entities)
		{
			const auto& movementOutput = movementOutputs[entity.movementTaskId];
			MovementSystem::UpdateComponent(entity.movementComponent, movementOutput);
			entity.transform.position = movementOutput.position;
			entity.transform.rotation = movementOutput.rotation;
		}
	}
}
