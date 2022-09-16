#pragma once
#include "Systems/EntitySystem.h"
#include "Systems/TurnSystem.h"
#include "SystemManager.h"

namespace game
{
	// Archetypes are used to execute behaviour for entities.
	template <typename ...Args>
	class EntityArchetype : public jlb::Archetype<vke::EngineData, Entity, Args...>
	{
	public:
		void OnPreUpdate(const vke::EngineData& info) override;

	private:
		typedef jlb::Archetype<vke::EngineData, Entity, Args...> Base;
	};

	template <typename ... Args>
	void EntityArchetype<Args...>::OnPreUpdate(const vke::EngineData& info)
	{
		Base::OnPreUpdate();

		const auto turnSys = info.systems.Get<TurnSystem>();
		if(turnSys->GetIfEndTickEvent())
		{
			jlb::ArchetypeView<Entity> entities;
			Base::template GetViews(entities);
			for (int32_t i = Base::GetCount() - 1; i >= 0; --i)
				if (entities[i].markedForDelete)
					Base::RemoveAt(i);
		}

		if(turnSys->GetIfBeginTickEvent())
		{
			const auto entitySys = info.systems.Get<EntitySystem>();

			jlb::ArchetypeView<Entity> entities;
			Base::template GetViews(entities);
			for (auto& entity : entities)
			{
				const size_t result = entitySys->TryAdd(info, &entity);
				assert(result != SIZE_MAX);
			}
		}
	}
}
