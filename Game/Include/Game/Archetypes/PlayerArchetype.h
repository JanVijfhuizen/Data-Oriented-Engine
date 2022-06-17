#pragma once
#include "Archetype.h"
#include "Components/Controller.h"
#include <Components/Character.h>
#include <Components/Collider.h>
#include "Components/Renderer.h"
#include "Systems/RenderSystem.h"
#include "Systems/EntityRenderSystem.h"
#include "Components/HumanoidBody.h"

namespace game
{
	class MovementSystem;

	struct Player final
	{
		Controller controller{};
		Collider collider{};
		Character character{};
		Renderer renderer{};
		HumanoidBodyInstance body{};
	};

	struct PlayerUpdateInfo final
	{
		EntityRenderSystem* entityRenderSystem;
		MovementSystem* movementSystem;
		glm::vec2 mousePosition;
	};

	class PlayerArchetype final : public Archetype<Player, PlayerUpdateInfo>
	{
	public:
		static void OnKeyInput(int key, int action, PlayerArchetype& instance);

		void Allocate(const EngineData& EngineData, SystemChain& chain) override;
		void Free(const EngineData& EngineData, SystemChain& chain) override;
		void Awake(const EngineData& EngineData, SystemChain& chain) override;

	private:
		using Archetype<Player, PlayerUpdateInfo>::IncreaseRequestedLength;

		Controller _playerController{};
		HumanoidBodyVisuals _bodyVisuals{};

		PlayerUpdateInfo OnPreEntityUpdate(const EngineData& EngineData, SystemChain& chain) override;
		void OnEntityUpdate(Player& entity, PlayerUpdateInfo& info) override;
	};
}
