#pragma once
#include "Archetype.h"
#include "Components/Controller.h"
#include <Components/Character.h>
#include <Components/Collider.h>
#include "Components/Renderer.h"
#include "Components/ShadowCaster.h"
#include "Components/Transform.h"
#include "Systems/RenderSystem.h"
#include "Graphics/RenderTask.h"
#include "Graphics/Animation.h"
#include "Components/Animator.h"

namespace game
{
	class AnimationSystem;

	struct Player
	{
		Animator animator{};
		Controller controller{};
		Collider collider{};
		Character character{};
		Renderer renderer{};
		ShadowCaster shadowCaster{};
		Transform transform{};
	};

	struct PlayerArchetypeCreateInfo final
	{
		RenderSystem<RenderTask>* renderSystem;
		AnimationSystem* animationSystem;
	};

	struct PlayerArchetypeUpdateInfo final
	{
		RenderSystem<RenderTask>* renderSystem;
		AnimationSystem* animationSystem;
		glm::vec2 mousePosition;
	};

	class PlayerArchetype final : public jlb::Archetype<Player, PlayerArchetypeCreateInfo, PlayerArchetypeUpdateInfo>
	{
	public:
		static void OnKeyInput(int key, int action, PlayerArchetype& instance);

		void Allocate(jlb::StackAllocator& allocator, size_t size, const Player& fillValue = {}) override;
		void Free(jlb::StackAllocator& allocator) override;
		void DefineResourceUsage(PlayerArchetypeCreateInfo& info) override;
		void Start(PlayerArchetypeCreateInfo& info) override;

	private:
		Controller _playerController{};
		Animation _testAnim{};

		void OnPreEntityUpdate(PlayerArchetypeUpdateInfo& info) override;
		void OnEntityUpdate(Player& entity, PlayerArchetypeUpdateInfo& info) override;
	};
}
