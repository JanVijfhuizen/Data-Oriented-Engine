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

	class PlayerArchetype final : public jlb::Archetype<Character, Collider, Controller, Renderer, ShadowCaster, Transform>
	{
	public:
		enum Ind
		{
			Character,
			Collider,
			Controller,
			Renderer,
			ShadowCaster,
			Transform
		};

		void Update(RenderSystem& renderSystem);
	};
}
