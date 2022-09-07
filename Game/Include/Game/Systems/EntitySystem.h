#pragma once
#include "VkEngine/Systems/TaskSystem.h"
#include <Entities/Entity.h>
#include "Heap.h"
#include "SparseSet.h"

namespace game
{
	/*
	Entities can be added here to be interacted with.
	The interaction system will use this entity system to update their variables based on interactions.
	*/
	class EntitySystem final : public vke::GameSystem
	{
	public:
		[[nodiscard]] Entity* operator[](size_t index) const;
		[[nodiscard]] bool Contains(const EntityId& id);

		void CreateEntity(Entity& entity);
		void DestroyEntity(Entity& entity);

	private:
		jlb::SparseSet<Entity*> _entities{};
		jlb::Heap<size_t> _open{};
		size_t _globalId = 0;

		void Allocate(const vke::EngineData& info) override;
		void Free(const vke::EngineData& info) override;
	};
}
