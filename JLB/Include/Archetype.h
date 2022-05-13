#pragma once
#include "Vector.h"

namespace jlb
{
	/// <summary>
	/// Class used to store and specify a type of entities.
	/// </summary>
	template <typename Entity, typename CreateInfo, typename UpdateInfo>
	class Archetype : public Vector<Entity>
	{
	public:
		using Base = Archetype<Entity, CreateInfo, UpdateInfo>;

		// Define the resources required from the systems.
		virtual void DefineResourceUsage(CreateInfo& info) = 0;
		// Start creating prototypes now that all the required resources have been set up.
		virtual void Start(CreateInfo& info) = 0;
		// Updates all entities in this archetype.
		void Update(UpdateInfo& info);

		Entity& Add(Entity& value) override;
		Entity& Add(Entity&& value = {}) override;
		void RemoveAt(size_t index) override;

	protected:
		// Update a single entity.
		virtual void OnPreEntityUpdate(UpdateInfo& info) {};
		virtual void OnEntityUpdate(Entity& entity, UpdateInfo& info) = 0;
		virtual void OnPostEntityUpdate(UpdateInfo& info) {}

		virtual void OnAdd(Entity& entity) {};
		virtual void OnRemove(Entity& entity, size_t index) {};

	private:
		using Vector<Entity>::AllocateAndCopy;
	};

	template <typename Entity, typename CreateInfo, typename UpdateInfo>
	void Archetype<Entity, CreateInfo, UpdateInfo>::Update(UpdateInfo& info)
	{
		OnPreEntityUpdate(info);
		for (auto& entity : *this)
			OnEntityUpdate(entity, info);
		OnPostEntityUpdate(info);
	}

	template <typename Entity, typename CreateInfo, typename UpdateInfo>
	Entity& Archetype<Entity, CreateInfo, UpdateInfo>::Add(Entity& entity)
	{
		auto& result = Vector<Entity>::Add(entity);
		OnAdd(result);
		return result;
	}

	template <typename Entity, typename CreateInfo, typename UpdateInfo>
	Entity& Archetype<Entity, CreateInfo, UpdateInfo>::Add(Entity&& entity)
	{
		auto& result = Vector<Entity>::Add(entity);
		OnAdd(result);
		return result;
	}

	template <typename Entity, typename CreateInfo, typename UpdateInfo>
	void Archetype<Entity, CreateInfo, UpdateInfo>::RemoveAt(const size_t index)
	{
		OnRemove(this->operator[](index), index);
		Vector<Entity>::RemoveAt(index);
	}
}
