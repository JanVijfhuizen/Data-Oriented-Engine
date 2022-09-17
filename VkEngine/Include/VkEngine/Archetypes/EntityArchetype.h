#pragma once
#include "Archetype.h"
#include "VkEngine/Entity.h"
#include <VkEngine/EngineData.h>

namespace vke
{
	// Archetypes are used to execute behaviour for entities.
	template <typename ...Args>
	class EntityArchetype : public jlb::Archetype<EngineData, Entity, Args...>
	{
	public:
		void OnPreUpdate(const EngineData& info) override;

	private:
		typedef jlb::Archetype<EngineData, Entity, Args...> Base;

		using Base::RemoveAt;
	};

	template <typename ... Args>
	void EntityArchetype<Args...>::OnPreUpdate(const EngineData& info)
	{
		Base::OnPreUpdate();

		jlb::ArchetypeView<Entity> entities;
		Base::template GetViews(entities);
		for (int32_t i = Base::GetCount() - 1; i >= 0; --i)
			if (entities[i].markedForDelete)
				Base::RemoveAt(i);
	}
}
