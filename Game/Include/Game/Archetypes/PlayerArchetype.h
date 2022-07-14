#pragma once
#include "Archetype.h"
#include "Entities/Player.h"

namespace game
{
	class PlayerArchetype final : public Archetype<Player>
	{
	public:
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			jlb::ArrayView<Player> entities) override;
		void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			jlb::ArrayView<Player> entities) override;
	};
}
