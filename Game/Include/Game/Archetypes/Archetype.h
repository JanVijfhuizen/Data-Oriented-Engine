#pragma once

namespace game
{
	/*
	Archetypes are used to execute behaviour for entities.
	While entities are stored in numerous ways (arrays, vectors, singles etc.),
	you can throw them through these archetypes to execute some sort of behaviour.
	*/
	template <typename Entity>
	class Archetype
	{
	public:
		virtual void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, jlb::ArrayView<Entity> entities){}
		virtual void EndFrame(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, jlb::ArrayView<Entity> entities) {}
	};
}
