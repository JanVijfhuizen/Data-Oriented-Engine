#pragma once
#include "EntityArchetype.h"
#include "Systems/CollisionSystem.h"
#include "Systems/MouseSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Components/Transform.h"
#include "VkEngine/Graphics/SubTexture.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "JlbMath.h"
#include "Entities/Character.h"
#include "VkEngine/Graphics/CameraUtils.h"

namespace game
{
	template <typename T>
	class CharacterArchetype : public EntityArchetype<T>
	{
	public:
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			jlb::Vector<T>& entities) override;
		void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			jlb::Vector<T>& entities) override;

	protected:
		struct CharacterInput final
		{
			glm::ivec2 movementDir{};
		};

		float scalingOnSelected = 0.5f;
		
		[[nodiscard]] virtual vke::SubTexture DefineSubTexture(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) = 0;
	};

	template <typename T>
	void CharacterArchetype<T>::PreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		jlb::Vector<T>& entities)
	{
		EntityArchetype<T>::PreUpdate(info, systems, entities);

		const auto collisionSys = systems.GetSystem<CollisionSystem>();
		const auto entityRenderSys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto mouseSys = systems.GetSystem<MouseSystem>();
		const auto movementSys = systems.GetSystem<MovementSystem>();
		const auto turnSys = systems.GetSystem<TurnSystem>();

		const size_t hoveredObj = mouseSys->GetHoveredObject();

		const auto subTexture = DefineSubTexture(info, systems);

		if (turnSys->GetIfBeginTickEvent())
			for (auto& entity : entities)
			{
				const auto base = reinterpret_cast<Character*>(&entity);

				auto& movementComponent = base->movementComponent;
				const auto& transform = base->transform;

				glm::vec2 collisionPos = transform.position;

				// Update movement task with new input.
				if (movementComponent.remaining == 0 || movementComponent.remaining == SIZE_MAX)
				{
					auto& input = base->input;
					const auto& dir = input.movementDir;
					
					const glm::vec2 from = jlb::math::RoundNearest(base->transform.position);
					const glm::vec2 delta = glm::vec2(dir);
					glm::vec2 to = from + delta;

					base->movementTaskId = SIZE_MAX;
					base->movementTileReservation = SIZE_MAX;
					movementComponent.remaining = SIZE_MAX;

					if (dir.x != 0 || dir.y != 0)
					{
						glm::ivec2 toRounded = jlb::math::RoundNearest(to);

						uint32_t outCollision;
						if (collisionSys->CheckIfTilesAreReserved(toRounded) != SIZE_MAX ||
							collisionSys->GetIntersections(toRounded, outCollision))
						{
							to = from;
						}
						else
						{
							base->movementTileReservation = collisionSys->ReserveTiles(toRounded);
							movementComponent.remaining = movementComponent.duration;
						}
					}

					movementComponent.from = from;
					movementComponent.to = to;
					movementComponent.rotation = transform.rotation;
					collisionPos = to;
				}

				// Collision task.
				CollisionTask collisionTask{};
				collisionTask.bounds = jlb::math::RoundNearest(collisionPos);
				collisionTask.bounds.layers = collisionLayerMain | collisionLayerInteractable;
				base->collisionTaskId = collisionSys->TryAdd(collisionTask);
				assert(base->collisionTaskId != SIZE_MAX);
			}

		for (auto& entity : entities)
		{
			const auto base = reinterpret_cast<Character*>(&entity);

			auto& movementComponent = base->movementComponent;
			const auto& transform = base->transform;

			vke::EntityRenderTask renderTask{};
			renderTask.subTexture = subTexture;
			renderTask.transform = transform;
			renderTask.transform.scale *= movementComponent.scaleMultiplier;
			const bool hovered = hoveredObj == base->mouseTaskId && hoveredObj != SIZE_MAX;
			renderTask.transform.scale *= 1.f + scalingOnSelected * static_cast<float>(hovered);

			const auto result = entityRenderSys->TryAdd(info, renderTask);
			base->movementTaskId = movementSys->TryAdd(info, movementComponent);

			{
				const auto& camera = entityRenderSys->camera;
				const bool culls = vke::Culls(camera.position, camera.pixelSize, transform.position, glm::vec2(transform.scale));
				base->mouseTaskId = SIZE_MAX;
				if (!culls)
				{
					jlb::FBounds bounds{ transform.position, glm::vec2(transform.scale) };
					base->mouseTaskId = mouseSys->TryAdd(info, bounds);
				}
			}
		}
	}

	template <typename T>
	void CharacterArchetype<T>::PostUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		jlb::Vector<T>& entities)
	{
		EntityArchetype<T>::PostUpdate(info, systems, entities);

		const auto movementSys = systems.GetSystem<MovementSystem>();

		for (auto& entity : entities)
		{
			const auto base = reinterpret_cast<Character*>(&entity);
			if (base->movementTaskId != SIZE_MAX)
			{
				auto& movementOutputs = movementSys->GetOutput();
				const auto& movementOutput = movementOutputs[base->movementTaskId];

				base->movementComponent = movementOutput;

				auto& transform = base->transform;
				transform.position = movementOutput.position;
				transform.rotation = movementOutput.rotation;
			}
		}
	}
}
