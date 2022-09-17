#pragma once
#include "Archetype.h"
#include "VkEngine/Components/Entity.h"
#include <VkEngine/EngineData.h>

namespace vke
{
	template <typename ...Args>
	class EntityArchetype : public jlb::Archetype<EngineData, Entity, Args...>
	{
	public:
		void OnPreUpdate(const EngineData& info) override;
		size_t Add(jlb::StackAllocator& levelAllocator, jlb::Tuple<Args...> components) override;

	private:
		typedef jlb::Archetype<EngineData, Entity, Args...> Base;
		using Base::RemoveAt;

		size_t _entityGlobalId = 0;
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

	template <typename ... Args>
	size_t EntityArchetype<Args...>::Add(jlb::StackAllocator& levelAllocator, jlb::Tuple<Args...> components)
	{
		jlb::Get<0>(components).id = _entityGlobalId++;
		return Base::Add(levelAllocator, components);
	}
}
