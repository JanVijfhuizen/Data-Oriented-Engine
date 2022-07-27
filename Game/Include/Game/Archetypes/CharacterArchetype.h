#pragma once
#include "Archetype.h"
#include "Components/MovementComponent.h"
#include "Systems/MouseSystem.h"
#include "Systems/MovementSystem.h"
#include "VkEngine/Components/Transform.h"
#include "VkEngine/Graphics/SubTexture.h"
#include "VkEngine/Systems/EntityRenderSystem.h"

namespace game
{
	struct Character final
	{
		vke::Transform transform{};
		MovementComponent movementComponent{};

		size_t movementTaskId = SIZE_MAX;
		size_t collisionTaskId = SIZE_MAX;
		size_t movementTileReservation = SIZE_MAX;
	};

	template <typename T>
	class CharacterArchetype : public Archetype<T>
	{
	protected:
		struct CharacterPreUpdateInfo final
		{
			vke::EntityRenderSystem* entityRenderSys = nullptr;
			MouseSystem* mouseSys = nullptr;
			MovementSystem* movementSys = nullptr;
			float scalingOnSelected = 0.5f;

			[[nodiscard]] bool GetIsHovered(const Character& character) const;
		};

		[[nodiscard]] CharacterPreUpdateInfo CreateCharacterPreUpdateInfo(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems);
		void PreUpdateCharacter(const vke::EngineData& info, Character& character, const CharacterPreUpdateInfo& updateInfo, const vke::SubTexture& subTexture);
	};

	template <typename T>
	bool CharacterArchetype<T>::CharacterPreUpdateInfo::GetIsHovered(const Character& character) const
	{
		const size_t hoveredObj = mouseSys->GetHoveredObject();
		return hoveredObj == character.collisionTaskId && hoveredObj != SIZE_MAX;
	}

	template <typename T>
	typename CharacterArchetype<T>::CharacterPreUpdateInfo CharacterArchetype<T>::CreateCharacterPreUpdateInfo(
		const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		CharacterPreUpdateInfo updateInfo{};
		updateInfo.entityRenderSys = systems.GetSystem<vke::EntityRenderSystem>();
		updateInfo.mouseSys = systems.GetSystem<MouseSystem>();
		updateInfo.movementSys = systems.GetSystem<MovementSystem>();

		return updateInfo;
	}

	template <typename T>
	void CharacterArchetype<T>::PreUpdateCharacter(const vke::EngineData& info, Character& character, 
		const CharacterPreUpdateInfo& updateInfo, const vke::SubTexture& subTexture)
	{
		const auto mouseSys = updateInfo.mouseSys;
		const auto movementSys = updateInfo.movementSys;
		
		const auto& movementComponent = character.movementComponent;
		const auto& transform = character.transform;

		vke::EntityRenderTask renderTask{};
		renderTask.subTexture = subTexture;
		renderTask.transform = transform;
		renderTask.transform.scale *= movementComponent.systemDefined.scaleMultiplier;
		const bool hovered = updateInfo.GetIsHovered(character);
		renderTask.transform.scale *= 1.f + updateInfo.scalingOnSelected * static_cast<float>(hovered);

		const auto result = updateInfo.entityRenderSys->TryAdd(info, renderTask);
		assert(result != SIZE_MAX);
		
		character.movementTaskId = movementSys->TryAdd(info, movementComponent);
	}
}
