#pragma once

namespace game
{
	template <typename Entity>
	class Archetype
	{
	public:
		virtual void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, jlb::ArrayView<Entity> entities){}
		virtual void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, jlb::ArrayView<Entity> entities) {}
	};
}
