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

	typedef jlb::Tuple<Character, Collider, Controller, Renderer, ShadowCaster, Transform> PlayerEntity;

	class PlayerArchetype final : public jlb::Archetype<PlayerEntity, RenderSystem>
	{
	public:
		enum Indices
		{
			Character,
			Collider,
			Controller,
			Renderer,
			ShadowCaster,
			Transform
		};

		void DefineResourceUsage(RenderSystem& renderSystem) override;

	protected:
		void OnUpdate(Entity& entity, RenderSystem& renderSystem) override;
	};
}
