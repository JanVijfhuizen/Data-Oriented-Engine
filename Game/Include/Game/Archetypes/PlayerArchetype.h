#pragma once
#include "Archetype.h"
#include "Components/Controller.h"
#include <Components/Character.h>
#include <Components/Collider.h>
#include "Components/Renderer.h"
#include "Components/ShadowCaster.h"
#include "Components/Transform.h"

namespace game
{
	class RenderSystem;

	struct Player
	{
		Controller controller;
		Collider collider;
		Character character;
		Renderer renderer;
		ShadowCaster shadowCaster;
		Transform transform;
	};

	struct PlayerArchetypeInfo final
	{
		RenderSystem* renderSystem;
	};

	class PlayerArchetype final : public jlb::Archetype<Player, PlayerArchetypeInfo>
	{
	public:
		void DefineResourceUsage(PlayerArchetypeInfo& info) override;
	protected:
		void OnUpdate(Player& entity, PlayerArchetypeInfo& info) override;
	};
}
