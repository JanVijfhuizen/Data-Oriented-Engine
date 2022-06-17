#include "pch.h"
#include "Archetypes/WallArchetype.h"
#include "Systems/CollisionSystem.h"
#include "Graphics/RenderConventions.h"

namespace game
{
	SubTexture WallArchetype::GenerateSubTexture(const Texture& texture) const
	{
		return texture::GenerateSubTexture(texture, renderConventions::ENTITY_SIZE, renderConventions::Player);
	}

	void WallArchetype::Allocate(const EngineData& EngineData, SystemChain& chain)
	{
		Archetype<Wall, WallUpdateInfo>::Allocate(EngineData, chain);

		const size_t length = GetLength();
		chain.Get<CollisionSystem>()->IncreaseRequestedLength(length, false);
		chain.Get<EntityRenderSystem>()->IncreaseRequestedLength(length);
	}

	void WallArchetype::Awake(const EngineData& EngineData, SystemChain& chain)
	{
		Archetype<Wall, WallUpdateInfo>::Awake(EngineData, chain);

		const auto& texture = chain.Get<EntityRenderSystem>()->GetTexture();
		_subTexture = GenerateSubTexture(texture);
	}

	void WallArchetype::Start(const EngineData& EngineData, SystemChain& chain)
	{
		Archetype<Wall, WallUpdateInfo>::Start(EngineData, chain);

		auto collisionSystem = chain.Get<CollisionSystem>();

		for (auto& wall : *this)
		{
			StaticCollisionTask collisionTask{};
			collisionTask.transform = wall.transform;
			collisionTask.collider = wall.collider;
			collisionSystem->AddStatic(collisionTask);
		}
	}

	WallUpdateInfo WallArchetype::OnPreEntityUpdate(const EngineData& EngineData, SystemChain& chain)
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

	Wall& WallArchetype::Add(const Wall& task)
	{
		auto& entity = TaskSystem<Wall>::Add(task);
		entity.renderer.subTexture = _subTexture;
		return entity;
	}
}
