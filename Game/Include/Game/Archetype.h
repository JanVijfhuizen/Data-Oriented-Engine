#pragma once
#include "TaskSystem.h"

namespace game
{
	/// <summary>
	/// Class used to store and specify a type of entities.
	/// </summary>
	template <typename Entity, typename UpdateInfo>
	class Archetype : public TaskSystem<Entity>
	{
	public:
		Entity& Add(Entity& entity) override;
		Entity& Add(Entity&& entity = {}) override;
		void RemoveAt(size_t index) override;

	protected:
		void Update(const EngineOutData& outData, SystemChain& chain) override;

		// Update a single entity.
		virtual UpdateInfo OnPreEntityUpdate(const EngineOutData& outData, SystemChain& chain) = 0;
		virtual void OnEntityUpdate(Entity& entity, UpdateInfo& info) = 0;
		virtual void OnPostEntityUpdate(const EngineOutData& outData, SystemChain& chain) {}

		virtual void OnAdd(Entity& entity) {};
		virtual void OnRemove(Entity& entity, size_t index) {};
	};

	template <typename Entity, typename UpdateInfo>
	Entity& Archetype<Entity, UpdateInfo>::Add(Entity& entity)
	{
		auto& result = jlb::Vector<Entity>::Add(entity);
		OnAdd(result);
		return result;
	}

	template <typename Entity, typename UpdateInfo>
	Entity& Archetype<Entity, UpdateInfo>::Add(Entity&& entity)
	{
		auto& result = jlb::Vector<Entity>::Add(entity);
		OnAdd(result);
		return result;
	}

	template <typename Entity, typename UpdateInfo>
	void Archetype<Entity, UpdateInfo>::RemoveAt(const size_t index)
	{
		OnRemove(this->operator[](index), index);
		jlb::Vector<Entity>::RemoveAt(index);
	}

	template <typename Entity, typename UpdateInfo>
	void Archetype<Entity, UpdateInfo>::Update(const EngineOutData& outData, SystemChain& chain)
	{
		auto info = OnPreEntityUpdate(outData, chain);
		for (auto& entity : *this)
			OnEntityUpdate(entity, info);
		OnPostEntityUpdate(outData, chain);
	}
}
