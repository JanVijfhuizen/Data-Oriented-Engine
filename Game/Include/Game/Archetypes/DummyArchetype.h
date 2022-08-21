#pragma once
#include "CharacterArchetype.h"
#include "Entities/DummyEntity.h"

namespace game::demo
{
	class DummyArchetype final : public CharacterArchetype<DummyEntity> 
	{
		void OnPreUpdate(const EntityArchetypeInfo& info, jlb::Systems<EntityArchetypeInfo> archetypes, jlb::NestedVector<DummyEntity>& entities) override;
		[[nodiscard]] vke::SubTexture DefineSubTextureSet(const vke::EngineData& info,
			jlb::Systems<vke::EngineData> systems) override;
	};
}
