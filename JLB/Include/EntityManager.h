#pragma once
#include "Heap.h"
#include "SparseSet.h"

namespace jlb
{
	class Archetype;

	struct EntityId final
	{
		size_t id = SIZE_MAX;
		size_t index = SIZE_MAX;

		[[nodiscard]] operator bool() const;
		[[nodiscard]] bool operator==(const EntityId& other) const;
	};

	struct Entity final
	{
		size_t id = SIZE_MAX;
		Archetype* archetype = nullptr;
		size_t archetypeIndex = SIZE_MAX;
	};

	class EntityManager final
	{
	public:
		void Allocate(StackAllocator& levelAllocator, size_t capacity);
		
		EntityId CreateEntity(Archetype& archetype);
		void DestroyEntity(const EntityId& id);
		[[nodiscard]] bool Validate(const EntityId& id) const;

	private:
		SparseSet<Entity> _entities{};
		Heap<size_t> _open{};
		size_t _entityGlobalId = 0;
	};

	inline EntityId::operator bool() const
	{
		return id != SIZE_MAX;
	}

	inline bool EntityId::operator==(const EntityId& other) const
	{
		return id == other.id;
	}

	inline void EntityManager::Allocate(StackAllocator& levelAllocator, const size_t capacity)
	{
		_entities.Allocate(levelAllocator, capacity);
		_open.Allocate(levelAllocator, capacity);
	}

	inline EntityId EntityManager::CreateEntity(Archetype& archetype)
	{
		const size_t index = _open.GetCount() == 0 ? _entities.GetCount() : _open.Pop();

		Entity entity{};
		entity.archetype = &archetype;
		// TODO add archetype index.
		entity.id = _entityGlobalId++;

		_entities.Insert(index, entity);

		EntityId id{};
		id.id = _entityGlobalId - 1;
		id.index = index;
		return id;
	}

	inline void EntityManager::DestroyEntity(const EntityId& id)
	{
		if (!Validate(id))
			return;
		size_t index = id.index;
		_entities.RemoveAt(index);
		_open.Insert(index, index);
	}

	inline bool EntityManager::Validate(const EntityId& id) const
	{
		return id && _entities[id.index].instance.id == id.id;
	}
}
