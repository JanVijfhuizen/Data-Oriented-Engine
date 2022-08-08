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
		const auto turnSys = systems.GetSystem<TurnSystem>();
		const auto entitySys = systems.GetSystem<EntitySystem>();
		const auto& entityTasks = entitySys->GetTasks();
		const auto count = static_cast<int32_t>(entities.GetCount());

		if(turnSys->GetIfEndTickEvent())
			for (int32_t i = count - 1; i >= 0; --i)
			{
				const auto base = reinterpret_cast<Entity*>(&entities[i]);
				base->data = base->entityTaskId == SIZE_MAX ? base->data : entityTasks[base->entityTaskId];
				if (const auto& data = base->data; data.markedForDelete)
					entities.RemoveAt(i);
			}

		if(turnSys->GetIfTickEvent())
			for (auto& entity : entities)
			{
				const auto base = reinterpret_cast<Entity*>(&entity);
				base->entityTaskId = entitySys->TryAdd(info, base->data);
				assert(base->entityTaskId != SIZE_MAX);
			}
	}

	template <typename T>
	void Archetype<T>::PostUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		jlb::Vector<T>& entities)
	{
			
	}
}
