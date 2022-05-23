﻿#pragma once
#include "Archetype.h"
#include "Components/Controller.h"
#include <Components/Character.h>
#include <Components/Collider.h>
#include "Components/Renderer.h"
#include "Components/Transform.h"
#include "Systems/RenderSystem.h"
#include "Systems/EntityRenderSystem.h"
#include "Graphics/Animation.h"
#include "Components/Animator.h"

namespace game
{
	class AnimationSystem;
	class MovementSystem;

	struct Player final
	{
		Animator animator{};
		Controller controller{};
		Collider collider{};
		Character character{};
		Renderer renderer{};
		Transform transform{};
	};

	struct PlayerUpdateInfo final
	{
		AnimationSystem* animationSystem;
		EntityRenderSystem* entityRenderSystem;
		MovementSystem* movementSystem;
		glm::vec2 mousePosition;
	};

	class PlayerArchetype final : public Archetype<Player, PlayerUpdateInfo>
	{
	public:
		static void OnKeyInput(int key, int action, PlayerArchetype& instance);

		void Allocate(const EngineOutData& outData, SystemChain& chain) override;
		void Free(const EngineOutData& outData, SystemChain& chain) override;
		void Awake(const EngineOutData& outData, SystemChain& chain) override;

	private:
		using Archetype<Player, PlayerUpdateInfo>::IncreaseRequestedLength;

		Controller _playerController{};
		Animation _testAnim{};

		PlayerUpdateInfo OnPreEntityUpdate(const EngineOutData& outData, SystemChain& chain) override;
		void OnEntityUpdate(Player& entity, PlayerUpdateInfo& info) override;
	};
}
