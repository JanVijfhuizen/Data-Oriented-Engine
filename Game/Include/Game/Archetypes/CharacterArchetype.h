﻿#pragma once
#include "Archetype.h"
#include "Systems/CollisionSystem.h"
#include "Systems/MouseSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Components/Transform.h"
#include "VkEngine/Graphics/SubTexture.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "JlbMath.h"
#include "Systems/EntitySystem.h"
#include "VkEngine/Graphics/CameraUtils.h"

namespace game
{
	template <typename T>
	class CharacterArchetype : public Archetype<T>
	{
	public:
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			jlb::Vector<T>& entities) override;
		void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			jlb::Vector<T>& entities) override;

	protected:
		float scalingOnSelected = 0.5f;

		struct CharacterInput final
		{
			glm::ivec2 movementDir{};
		};

		struct CharacterUpdateInfo final
		{
			CollisionSystem* collisionSys = nullptr;
			EntitySystem* entitySys = nullptr;
			vke::EntityRenderSystem* entityRenderSys = nullptr;
			MouseSystem* mouseSys = nullptr;
			MovementSystem* movementSys = nullptr;
			TurnSystem* turnSys = nullptr;

			[[nodiscard]] bool GetIsHovered(const Character& character) const;
		};

		[[nodiscard]] CharacterUpdateInfo CreateCharacterPreUpdateInfo(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems);
		void PreUpdateCharacter(const vke::EngineData& info, Character& character, const CharacterUpdateInfo& updateInfo, 
			const vke::SubTexture& subTexture, const CharacterInput& input);
		void PostUpdateCharacter(const vke::EngineData& info, Character& character, const CharacterUpdateInfo& updateInfo);
	};

	template <typename T>
	void CharacterArchetype<T>::PreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		jlb::Vector<T>& entities)
	{
		Archetype<T>::PreUpdate(info, systems, entities);
	}

	template <typename T>
	void CharacterArchetype<T>::PostUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		jlb::Vector<T>& entities)
	{
		Archetype<T>::PostUpdate(info, systems, entities);
	}

	template <typename T>
	bool CharacterArchetype<T>::CharacterUpdateInfo::GetIsHovered(const Character& character) const
	{
		const size_t hoveredObj = mouseSys->GetHoveredObject();
		return hoveredObj == character.mouseTaskId && hoveredObj != SIZE_MAX;
	}

	template <typename T>
	typename CharacterArchetype<T>::CharacterUpdateInfo CharacterArchetype<T>::CreateCharacterPreUpdateInfo(
		const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		CharacterUpdateInfo updateInfo{};
		updateInfo.collisionSys = systems.GetSystem<CollisionSystem>();
		updateInfo.entitySys = systems.GetSystem<EntitySystem>();
		updateInfo.entityRenderSys = systems.GetSystem<vke::EntityRenderSystem>();
		updateInfo.mouseSys = systems.GetSystem<MouseSystem>();
		updateInfo.movementSys = systems.GetSystem<MovementSystem>();
		updateInfo.turnSys = systems.GetSystem<TurnSystem>();

		return updateInfo;
	}

	template <typename T>
	void CharacterArchetype<T>::PreUpdateCharacter(const vke::EngineData& info, Character& character, 
		const CharacterUpdateInfo& updateInfo, const vke::SubTexture& subTexture, const CharacterInput& input)
	{
		const auto collisionSys = updateInfo.collisionSys;
		const auto entitySys = updateInfo.entitySys;
		const auto entityRenderSys = updateInfo.entityRenderSys;
		const auto mouseSys = updateInfo.mouseSys;
		const auto movementSys = updateInfo.movementSys;
		const auto turnSys = updateInfo.turnSys;
		
		auto& movementComponent = character.movementComponent;
		const auto& transform = character.transform;

		vke::EntityRenderTask renderTask{};
		renderTask.subTexture = subTexture;
		renderTask.transform = transform;
		renderTask.transform.scale *= movementComponent.systemDefined.scaleMultiplier;
		const bool hovered = updateInfo.GetIsHovered(character);
		renderTask.transform.scale *= 1.f + scalingOnSelected * static_cast<float>(hovered);

		const auto result = updateInfo.entityRenderSys->TryAdd(info, renderTask);
		character.movementTaskId = movementSys->TryAdd(info, movementComponent);

		{
			const auto& camera = entityRenderSys->camera;
			const bool culls = vke::Culls(camera.position, camera.pixelSize, transform.position, glm::vec2(transform.scale));
			character.mouseTaskId = SIZE_MAX;
			if (!culls)
			{
				jlb::FBounds bounds{ transform.position, glm::vec2(transform.scale) };
				character.mouseTaskId = mouseSys->TryAdd(info, bounds);
			}
		}

		if(turnSys->GetIfTickEvent())
		{
			const auto& movementSystemDefined = movementComponent.systemDefined;
			// No need to store this since we assume the player is always the first entity.
			character.entityTaskId = entitySys->TryAdd(info, character.entity);
			
			// Update movement task with new input.
			if (movementSystemDefined.remaining <= 1)
			{
				const auto& dir = input.movementDir;

				character.movementTileReservation = SIZE_MAX;
				movementComponent.Finish();

				const glm::vec2 from = glm::vec2(jlb::math::RoundNearest(character.transform.position));
				const glm::vec2 delta = glm::vec2(dir);
				glm::vec2 to = from + delta;
				
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
						character.movementTileReservation = collisionSys->ReserveTiles(toRounded);
						movementComponent.Build();
					}
				}

				auto& movementUserDefined = movementComponent.userDefined;
				movementUserDefined.from = from;
				movementUserDefined.to = to;
				movementUserDefined.rotation = transform.rotation;

				// Collision task.
				CollisionTask collisionTask{};
				collisionTask.bounds = jlb::math::RoundNearest(to);
				collisionTask.bounds.layers = collisionLayerMain | collisionLayerInteractable;
				character.collisionTaskId = collisionSys->TryAdd(collisionTask);
				assert(character.collisionTaskId != SIZE_MAX);
			}
		}
	}

	template <typename T>
	void CharacterArchetype<T>::PostUpdateCharacter(const vke::EngineData& info, 
		Character& character, const CharacterUpdateInfo& updateInfo)
	{
		if (character.movementTaskId != SIZE_MAX)
		{
			const auto movementSys = updateInfo.movementSys;
			auto& movementOutputs = movementSys->GetOutput();
			const auto& movementOutput = movementOutputs[character.movementTaskId];

			character.movementComponent.Update(movementOutput);

			auto& transform = character.transform;
			transform.position = movementOutput.position;
			transform.rotation = movementOutput.rotation;
		}
	}
}
