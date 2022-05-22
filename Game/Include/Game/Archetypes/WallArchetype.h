#pragma once
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
	protected:
		// TODO: make virtual to support multiple wall types.
		SubTexture GenerateSubTexture(const Texture& texture) const;

		void Allocate(const EngineOutData& outData, SystemChain& chain) override;
		void Start(const EngineOutData& outData, SystemChain& chain) override;

		WallUpdateInfo OnPreEntityUpdate(const EngineOutData& outData, SystemChain& chain) override;
		void OnEntityUpdate(Wall& entity, WallUpdateInfo& info) override;

		void OnAdd(Wall& entity) override;
	private:
		SubTexture _subTexture;
	};
}
