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
		Add(entity);
	}

	void EntitySystem::DestroyEntity(Entity& entity)
	{
		_open.Insert(entity.id, entity.id);
	}

	void EntitySystem::Add(const Entity& entity)
	{
		_entities.Insert(entity.id, entity.data);
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

	void EntitySystem::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		System<vke::EngineData>::PreUpdate(info, systems);

		const auto turnSys = systems.GetSystem<TurnSystem>();
		if (turnSys->GetIfBeginTickEvent())
			_entities.Clear();
	}
}
