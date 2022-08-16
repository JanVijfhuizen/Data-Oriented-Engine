﻿#include "pch.h"
#include "Systems/EntitySystem.h"
#include "Systems/TurnSystem.h"

namespace game
{
	EntityData& EntitySystem::operator[](const size_t index) const
	{
		return _entities[index].instance;
	}

	bool EntitySystem::Contains(const EntityId& id)
	{
		return id ? _entities.Contains(id.index) && _entities[id.index].sparseIndex == id.id : false;
	}

	void EntitySystem::CreateEntity(Entity& entity)
	{
		auto& id = entity.id;
		id.index = _open.GetCount() > 0 ? _open.Pop() : _entities.GetCount();
		id.id = _globalId++;
		_entities.Insert(id.index, entity.data);
	}

	void EntitySystem::DestroyEntity(Entity& entity)
	{
		auto& id = entity.id;
		_open.Insert(id.index, id.index);
		_entities.RemoveAt(id.index);
	}

	void EntitySystem::UpdateEntity(Entity& entity) const
	{
		auto& id = entity.id;
		auto& instance = _entities[id.index];
		entity.BuildData();
		instance.instance = entity.data;
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
