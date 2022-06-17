﻿#pragma once
#include "Components/Collider.h"
#include "Components/Renderer.h"
#include "Components/Transform.h"
#include "Systems/EntityRenderSystem.h"
#include "Archetype.h"

namespace game
{
	struct Wall final
	{
		Collider collider{};
		Renderer renderer{};
		Transform transform{};
	};

	struct WallUpdateInfo final
	{
		EntityRenderSystem* entityRenderSystem;
	};

	class WallArchetype : public Archetype<Wall, WallUpdateInfo>
	{
	public:
		Wall& Add(const Wall& task = {}) override;

	protected:
		// TODO: make virtual to support multiple wall types.
		[[nodiscard]] SubTexture GenerateSubTexture(const Texture& texture) const;

		void Allocate(const EngineData& EngineData, SystemChain& chain) override;
		void Awake(const EngineData& EngineData, SystemChain& chain) override;
		void Start(const EngineData& EngineData, SystemChain& chain) override;

		WallUpdateInfo OnPreEntityUpdate(const EngineData& EngineData, SystemChain& chain) override;
		void OnEntityUpdate(Wall& entity, WallUpdateInfo& info) override;

	private:
		SubTexture _subTexture;
	};
}
