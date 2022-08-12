#include "pch.h"
#include "Systems/EntitySystem.h"
#include "Systems/TurnSystem.h"

namespace game
{
	EntityData& EntitySystem::operator[](const size_t index) const
	{
		return _entities[index].instance;
	}

	void EntitySystem::CreateEntity(Entity& entity)
	{
		entity.id = _open.GetCount() > 0 ? _open.Pop() : _entities.GetCount();
		_entities.Insert(entity.id, entity.data);
	}

	void EntitySystem::DestroyEntity(Entity& entity)
	{
		_open.Insert(entity.id, entity.id);
		_entities.RemoveAt(entity.id);
	}

	void EntitySystem::UpdateEntity(const Entity& entity) const
	{
		_entities[entity.id].instance = entity.data;
	}

	void EntitySystem::Allocate(const vke::EngineData& info)
	{
		System<vke::EngineData>::Allocate(info);
		_entities.Allocate(*info.allocator, ENTITY_CAPACITY);
		_open.Allocate(*info.allocator, ENTITY_CAPACITY);
	}

	void EntitySystem::Free(const vke::EngineData& info)
	{
		_open.Free(*info.allocator);
		_entities.Free(*info.allocator);
		System<vke::EngineData>::Free(info);
	}
}
