#pragma once
#include "CharacterArchetype.h"
#include "Entities/DummyEntity.h"

namespace game::demo
{
	class DummyArchetype final : public CharacterArchetype<DummyEntity> 
	{
	public:
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, jlb::Vector<DummyEntity>& entities) override;
	private:
		[[nodiscard]] vke::SubTexture DefineSubTexture(const vke::EngineData& info,
			jlb::Systems<vke::EngineData> systems) override;
	};
}
