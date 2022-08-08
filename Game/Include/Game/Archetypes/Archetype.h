#pragma once

namespace game
{
	/*
	Archetypes are used to execute behaviour for entities.
	*/
	template <typename Entity>
	class Archetype
	{
	public:
		virtual void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, jlb::Vector<Entity>& entities){}
		virtual void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, jlb::Vector<Entity>& entities) {}
	};
}
