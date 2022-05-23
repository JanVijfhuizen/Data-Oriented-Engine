#include "pch.h"
#include "Archetypes/WallArchetype.h"
#include "Systems/CollisionSystem.h"
#include "Graphics/RenderConventions.h"

namespace game
{
	SubTexture WallArchetype::GenerateSubTexture(const Texture& texture) const
	{
		return TextureHandler::GenerateSubTexture(texture, RenderConventions::ENTITY_SIZE, RenderConventions::Player);
	}

	void WallArchetype::Allocate(const EngineOutData& outData, SystemChain& chain)
	{
		Archetype<Wall, WallUpdateInfo>::Allocate(outData, chain);

		const size_t length = GetLength();
		chain.Get<CollisionSystem>()->IncreaseRequestedLength(length, false);
		chain.Get<EntityRenderSystem>()->IncreaseRequestedLength(length);
	}

	void WallArchetype::Awake(const EngineOutData& outData, SystemChain& chain)
	{
		Archetype<Wall, WallUpdateInfo>::Awake(outData, chain);

		const auto& texture = chain.Get<EntityRenderSystem>()->GetTexture();
		_subTexture = GenerateSubTexture(texture);
	}

	void WallArchetype::Start(const EngineOutData& outData, SystemChain& chain)
	{
		Archetype<Wall, WallUpdateInfo>::Start(outData, chain);

		auto collisionSystem = chain.Get<CollisionSystem>();

		for (auto& wall : *this)
		{
			StaticCollisionTask collisionTask{};
			collisionTask.transform = wall.transform;
			collisionTask.collider = wall.collider;
			collisionSystem->AddStatic(collisionTask);
		}
	}

	WallUpdateInfo WallArchetype::OnPreEntityUpdate(const EngineOutData& outData, SystemChain& chain)
	{
		WallUpdateInfo info{};
		info.entityRenderSystem = chain.Get<EntityRenderSystem>();
		return info;
	}

	void WallArchetype::OnEntityUpdate(Wall& entity, WallUpdateInfo& info)
	{
		auto& transform = entity.transform;
		auto& renderer = entity.renderer;

		EntityRenderTask renderTask{};
		auto& taskTransform = renderTask.transform;
		taskTransform = transform;
		renderTask.subTexture = renderer.subTexture;
		info.entityRenderSystem->Add(renderTask);
	}

	void WallArchetype::OnAdd(Wall& entity)
	{
		entity.renderer.subTexture = _subTexture;
	}
}
