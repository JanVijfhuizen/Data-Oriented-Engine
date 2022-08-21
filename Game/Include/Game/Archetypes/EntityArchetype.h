﻿#pragma once
#include "Entities/Entity.h"
#include "Systems/EntitySystem.h"
#include "Systems/TurnSystem.h"
#include "SystemManager.h"

namespace game
{
	struct EntityArchetypeInfo final
	{
		vke::EngineData const* vkeInfo = nullptr;
		jlb::StackAllocator* sceneAllocator = nullptr;
		jlb::Systems<vke::EngineData> systems{};
	};

	// Archetypes are used to execute behaviour for entities.
	template <typename T>
	class EntityArchetype : public jlb::System<EntityArchetypeInfo>
	{
	public:
		[[nodiscard]] const jlb::NestedVector<T>& GetEntities() const;

	protected:
		void Allocate(const EntityArchetypeInfo& info) override;

		virtual void OnPreUpdate(const EntityArchetypeInfo& info,
			jlb::Systems<EntityArchetypeInfo> archetypes, jlb::NestedVector<T>& entities);
		virtual void OnPostUpdate(const EntityArchetypeInfo& info,
			jlb::Systems<EntityArchetypeInfo> archetypes, jlb::NestedVector<T>& entities);

		[[nodiscard]] virtual T DefinePrototype(const EntityArchetypeInfo& info);
		[[nodiscard]] virtual size_t DefineCapacity() const;
		[[nodiscard]] virtual size_t DefineNestedCapacity() const;

	private:
		jlb::NestedVector<T> _entities{};

		void PreUpdate(const EntityArchetypeInfo& info, jlb::Systems<EntityArchetypeInfo> archetypes) override;
		void PostUpdate(const EntityArchetypeInfo& info, jlb::Systems<EntityArchetypeInfo> archetypes) override;
	};

	template <typename T>
	const jlb::NestedVector<T>& EntityArchetype<T>::GetEntities() const
	{
		return _entities;
	}

	template <typename T>
	void EntityArchetype<T>::Allocate(const EntityArchetypeInfo& info)
	{
		System<EntityArchetypeInfo>::Allocate(info);
		auto& allocator = *info.sceneAllocator;
		_entities.Allocate(allocator, DefineCapacity(), DefineNestedCapacity());
	}

	template <typename T>
	void EntityArchetype<T>::OnPreUpdate(const EntityArchetypeInfo& info,
		const jlb::Systems<EntityArchetypeInfo> archetypes, jlb::NestedVector<T>& entities)
	{
		const auto turnSys = info.systems.GetSystem<TurnSystem>();
		const auto entitySys = info.systems.GetSystem<EntitySystem>();
		const auto count = static_cast<int32_t>(entities.GetCount());

		if (turnSys->GetIfEndTickEvent())
			for (int32_t i = count - 1; i >= 0; --i)
			{
				const auto base = static_cast<Entity*>(&entities[i]);
				base->data = entitySys->operator[](base->id.index);
				if (const auto& data = base->data; data.markedForDelete)
				{
					entitySys->DestroyEntity(*base);
					entities.RemoveAt(i);
				}
			}

		if (turnSys->GetIfBeginTickEvent())
			for (auto& entity : entities)
			{
				const auto base = reinterpret_cast<Entity*>(&entity);
				entitySys->UpdateEntity(*base);
			}
	}

	template <typename T>
	void EntityArchetype<T>::OnPostUpdate(const EntityArchetypeInfo& info,
		const jlb::Systems<EntityArchetypeInfo> archetypes, jlb::NestedVector<T>& entities)
	{
	}

	template <typename T>
	void EntityArchetype<T>::PreUpdate(const EntityArchetypeInfo& info, const jlb::Systems<EntityArchetypeInfo> archetypes)
	{
		System<EntityArchetypeInfo>::PreUpdate(info, archetypes);
		OnPreUpdate(info, archetypes, _entities);
	}

	template <typename T>
	void EntityArchetype<T>::PostUpdate(const EntityArchetypeInfo& info,
		const jlb::Systems<EntityArchetypeInfo> archetypes)
	{
		System<EntityArchetypeInfo>::PostUpdate(info, archetypes);
		OnPostUpdate(info, archetypes, _entities);
	}

	template <typename T>
	T EntityArchetype<T>::DefinePrototype(const EntityArchetypeInfo& info)
	{
		return T();
	}

	template <typename T>
	size_t EntityArchetype<T>::DefineCapacity() const
	{
		return 8;
	}

	template <typename T>
	size_t EntityArchetype<T>::DefineNestedCapacity() const
	{
		return 8;
	}
}
