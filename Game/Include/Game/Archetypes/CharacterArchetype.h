#pragma once
#include "Curve.h"
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
#include "Systems/PickupSystem.h"
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
		
		[[nodiscard]] virtual vke::SubTexture DefineSubTextureSet(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) = 0;
		[[nodiscard]] virtual size_t DefineSubTextureSetLength() const;
		[[nodiscard]] virtual glm::vec2 GetRightHandOffset() const;
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
		const auto pickupSystem = systems.GetSystem<PickupSystem>();
		const auto turnSys = systems.GetSystem<TurnSystem>();

		const size_t hoveredObj = mouseSys->GetHoveredObject();
		const bool ifBeginTickEvent = turnSys->GetIfBeginTickEvent();

		if (ifBeginTickEvent)
			for (auto& entity : entities)
			{
				const auto base = static_cast<Character*>(&entity);

				auto& movementComponent = base->movementComponent;
				const auto& transform = base->transform;

				glm::vec2 collisionPos = transform.position;

				bool occupied = false;
				if(base->pickupComponent.active)
					occupied = true;

				// Update movement task with new input.
				if (!occupied && movementComponent.outRemaining == 0)
				{
					auto& input = base->input;
					const auto& dir = input.movementDir;
					
					const glm::vec2 from = jlb::math::RoundNearest(transform.position);
					const glm::vec2 delta = glm::vec2(dir);
					glm::vec2 to = from + delta;
					
					base->movementTileReservation = SIZE_MAX;

					if (dir.x != 0 || dir.y != 0)
					{
						glm::ivec2 toRounded = jlb::math::RoundNearest(to);

						collisionPos = from;
						uint32_t outCollision;
						if (collisionSys->CheckIfTilesAreReserved(toRounded) == SIZE_MAX &&
							!collisionSys->GetIntersections(toRounded, outCollision))
						{
							base->movementTileReservation = collisionSys->ReserveTilesThisTurn(toRounded);
							movementComponent.outRemaining = movementComponent.inDuration;
							movementComponent.active = true;
							collisionPos = to;
						}
					}

					movementComponent.inFrom = from;
					movementComponent.inTo = to;
					movementComponent.outRotation = transform.rotation;
				}

				// Collision task.
				CollisionTask collisionTask{};
				collisionTask.bounds = jlb::math::RoundNearest(collisionPos);
				collisionTask.bounds.layers = collisionLayerMain | collisionLayerInteractable;
				base->collisionTaskId = collisionSys->TryAdd(collisionTask);
				assert(base->collisionTaskId != SIZE_MAX);
			}

		{
			const auto subTexture = DefineSubTextureSet(info, systems);
			const size_t subTextureLength = DefineSubTextureSetLength();

			const auto headSubTexture = vke::texture::GetSubTexture(subTexture, subTextureLength, 0);
			const auto handSubTexture = vke::texture::GetSubTexture(subTexture, subTextureLength, 1);
			const float hPi = jlb::math::PI * .5f;
			const float dPi = jlb::math::PI * 2;

			const float tickLerp = turnSys->GetTickLerp();
			auto curve = jlb::CreateCurveOvershooting();
			const auto handOffset = GetRightHandOffset();
			const float handLerpAngle = DoubleCurveEvaluate(tickLerp, curve, curve) * dPi * .1f;

			for (auto& entity : entities)
			{
				const auto base = reinterpret_cast<Character*>(&entity);

				const auto& movementComponent = base->movementComponent;
				const auto& pickupComponent = base->pickupComponent;

				const auto& transform = base->transform;
				base->movementTaskId = movementSys->TryAdd(info, movementComponent);
				
				if (base->pickupComponent.active)
				{
					base->pickupTaskId = pickupSystem->TryAdd(info, base->pickupComponent);
					assert(base->pickupTaskId != SIZE_MAX);
				}

				{
					const auto& camera = entityRenderSys->camera;
					const bool culls = vke::Culls(camera.position, camera.pixelSize, transform.position, glm::vec2(transform.scale));
					base->mouseTaskId = SIZE_MAX;
					if (!culls)
					{
						jlb::FBounds bounds{ transform.position, glm::vec2(transform.scale) };
						base->mouseTaskId = mouseSys->TryAdd(info, bounds);

						vke::EntityRenderTask renderTask{};
						renderTask.transform = transform;
						renderTask.transform.scale *= movementComponent.outScaleMultiplier;
						const bool hovered = hoveredObj == base->mouseTaskId && hoveredObj != SIZE_MAX;
						renderTask.transform.scale *= 1.f + scalingOnSelected * static_cast<float>(hovered);

						glm::vec2 v1 = jlb::math::Rotate(handOffset, transform.rotation);
						glm::vec2 v2 = jlb::math::Rotate(handOffset * glm::vec2(-1.f, 1.f), transform.rotation);
						v1 = movementComponent.active ? jlb::math::Rotate(v1, handLerpAngle) : v1;
						v2 = movementComponent.active ? jlb::math::Rotate(v2, handLerpAngle) : v2;

						v1 += transform.position;
						v2 += transform.position;

						// Render the hands.
						const bool pickupOngoing = pickupComponent.active && !ifBeginTickEvent;
						renderTask.transform.position = pickupOngoing ? pickupComponent.handPositions[0] : v1;
						renderTask.subTexture = handSubTexture;
						auto result = entityRenderSys->TryAdd(info, renderTask);

						renderTask.transform.position = pickupOngoing ? pickupComponent.handPositions[1] : v2;
						result = entityRenderSys->TryAdd(info, renderTask);

						// Render the head.
						renderTask.transform.position = transform.position;
						renderTask.subTexture = headSubTexture;
						result = entityRenderSys->TryAdd(info, renderTask);
					}
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
		const auto pickupSys = systems.GetSystem<PickupSystem>();

		for (auto& entity : entities)
		{
			const auto base = reinterpret_cast<Character*>(&entity);
			if (base->movementTaskId != SIZE_MAX)
			{
				auto& outputs = movementSys->GetOutput();
				const auto& output = outputs[base->movementTaskId];
				base->movementComponent = output;

				auto& transform = base->transform;
				transform.position = output.outPosition;
				transform.rotation = output.outRotation;
			}
			if(base->pickupTaskId != SIZE_MAX)
			{
				auto& outputs = pickupSys->GetOutput();
				const auto& output = outputs[base->pickupTaskId];
				base->pickupComponent = output;
			}
		}
	}

	template <typename T>
	size_t CharacterArchetype<T>::DefineSubTextureSetLength() const
	{
		return 2;
	}

	template <typename T>
	glm::vec2 CharacterArchetype<T>::GetRightHandOffset() const
	{
		const float pct = 1.f / static_cast<float>(vke::PIXEL_SIZE_ENTITY);
		return {pct * 6, pct * 2};
	}
}
