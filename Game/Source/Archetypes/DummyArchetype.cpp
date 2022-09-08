﻿#include "pch.h"
#include "Archetypes/DummyArchetype.h"
#include "Systems/ResourceSystem.h"

namespace game::demo
{
	void DummyArchetype::OnPreUpdate(const EntityArchetypeInfo& info, jlb::Systems<EntityArchetypeInfo> archetypes,
		jlb::NestedVector<DummyEntity>& entities)
	{
		for (auto& entity : entities)
		{
			auto& characterInput = entity.input;
			characterInput.movementDir.x = rand() % 3 - 1;
			characterInput.movementDir.y = rand() % 3 - 1;
		}

		CharacterArchetype<DummyEntity>::OnPreUpdate(info, archetypes, entities);
	}

	vke::SubTexture DummyArchetype::DefineSubTextureSet(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems)
	{
		const auto resourceSys = systems.Get<ResourceSystem>();
		const auto subTexture = resourceSys->GetSubTexture(ResourceSystem::EntitySubTextures::humanoid);
		return subTexture;
	}
}
