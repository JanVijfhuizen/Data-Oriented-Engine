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

	struct PlayerArchetypeInfo final
	{
		RenderSystem* renderSystem;
	};

	class PlayerArchetype final : public jlb::Archetype<PlayerArchetypeInfo, Character, Collider, Controller, Renderer, ShadowCaster, Transform>
	{
	public:
		void DefineResourceUsage(PlayerArchetypeInfo& info) override;
	protected:
		void OnUpdate(PlayerArchetypeInfo& info, Character&, Collider&, Controller&, Renderer&, ShadowCaster&,
			Transform&) override;
	};
}
