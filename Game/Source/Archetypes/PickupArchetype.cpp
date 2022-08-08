#include "pch.h"
#include "Archetypes/PickupArchetype.h"

#include "Bounds.h"
#include "JlbMath.h"
#include "Systems/CollisionSystem.h"
#include "Systems/MouseSystem.h"
#include "Systems/ResourceManager.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Graphics/CameraUtils.h"
#include "VkEngine/Systems/EntityRenderSystem.h"

namespace game
{
	void PickupArchetype::PreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::ArrayView<Pickup> entities)
	{
		Archetype<Pickup>::PreUpdate(info, systems, entities);

		const auto entityRenderSys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto mouseSys = systems.GetSystem<MouseSystem>();
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto turnSys = systems.GetSystem<TurnSystem>();
		const auto& camera = entityRenderSys->camera;

		const auto subTexture = resourceSys->GetSubTexture(ResourceManager::EntitySubTextures::pickup);
		vke::EntityRenderTask task{};
		task.subTexture = subTexture;

		for (auto& entity : entities)
		{
			const auto& transform = entity.transform;
			const bool culls = vke::Culls(camera.position, camera.pixelSize, transform.position, glm::vec2(transform.scale));
			entity.mouseTaskId = SIZE_MAX;
			if (!culls)
			{
				jlb::FBounds bounds{ transform.position, glm::vec2(transform.scale) };
				entity.mouseTaskId = mouseSys->TryAdd(info, bounds);
			}

			task.transform = transform;
			const auto result = entityRenderSys->TryAdd(info, task);
		}

		if(turnSys->GetIfTickEvent())
		{
			const auto collisionSys = systems.GetSystem<CollisionSystem>();

			for (auto& entity : entities)
			{
				const auto& transform = entity.transform;

				// Collision task.
				CollisionTask collisionTask{};
				collisionTask.bounds = jlb::math::RoundNearest(transform.position);
				collisionTask.bounds.layers = collisionLayerMain | collisionLayerInteractable;
				entity.collisionTaskId = collisionSys->TryAdd(collisionTask);
				assert(entity.collisionTaskId != SIZE_MAX);
			}
		}
	}
}
