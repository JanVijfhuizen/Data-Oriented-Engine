#pragma once
#include "Entities/Entity.h"
#include "Systems/EntitySystem.h"
#include "Systems/TurnSystem.h"

namespace game
{
	/*
	Archetypes are used to execute behaviour for entities.
	*/
	template <typename T>
	class EntityArchetype
	{
	public:
		virtual void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, jlb::Vector<T>& entities);
		virtual void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, jlb::Vector<T>& entities);
	};

	template <typename T>
	void EntityArchetype<T>::PreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		jlb::Vector<T>& entities)
	{
		const auto turnSys = systems.GetSystem<TurnSystem>();
		const auto entitySys = systems.GetSystem<EntitySystem>();
		const auto count = static_cast<int32_t>(entities.GetCount());

		if(turnSys->GetIfEndTickEvent())
			for (int32_t i = count - 1; i >= 0; --i)
			{
				const auto base = reinterpret_cast<Entity*>(&entities[i]);
				base->data = entitySys->operator[](base->id.index);
				if (const auto& data = base->data; data.markedForDelete)
				{
					entitySys->DestroyEntity(*base);
					entities.RemoveAt(i);
				}
			}

		if(turnSys->GetIfBeginTickEvent())
			for (auto& entity : entities)
			{
				const auto base = reinterpret_cast<Entity*>(&entity);
				entitySys->UpdateEntity(*base);
			}
	}

	template <typename T>
	void EntityArchetype<T>::PostUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		jlb::Vector<T>& entities)
	{
			
	}
}
