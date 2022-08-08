#pragma once
#include "Entities/Entity.h"
#include "Systems/EntitySystem.h"

namespace game
{
	/*
	Archetypes are used to execute behaviour for entities.
	*/
	template <typename T>
	class Archetype
	{
	public:
		virtual void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, jlb::Vector<T>& entities);
		virtual void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, jlb::Vector<T>& entities);
	};

	template <typename T>
	void Archetype<T>::PreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		jlb::Vector<T>& entities)
	{
		const auto& entitySys = systems.GetSystem<EntitySystem>();

		const auto count = static_cast<int32_t>(entities.GetCount());
		for (int32_t i = count - 1; i >= 0; --i)
		{
			const auto base = reinterpret_cast<Entity*>(&entities[i]);
			if (const auto& data = base->data; data.markedForDelete)
				entities.RemoveAt(i);
		}

		for (auto& entity : entities)
		{
			const auto base = reinterpret_cast<Entity*>(&entity);
			base->entityTaskId = entitySys->TryAdd(info, base->data);
			assert(base->entityTaskId);
		}
	}

	template <typename T>
	void Archetype<T>::PostUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		jlb::Vector<T>& entities)
	{
			
	}
}
