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
	protected:
		void Update(const EngineData& EngineData, SystemChain& chain) override;

		// Update a single entity.
		virtual UpdateInfo OnPreEntityUpdate(const EngineData& EngineData, SystemChain& chain) = 0;
		virtual void OnEntityUpdate(Entity& entity, UpdateInfo& info) = 0;
		virtual void OnPostEntityUpdate(const EngineData& EngineData, SystemChain& chain) {}
	};

	template <typename Entity, typename UpdateInfo>
	void Archetype<Entity, UpdateInfo>::Update(const EngineData& EngineData, SystemChain& chain)
	{
		auto info = OnPreEntityUpdate(EngineData, chain);
		for (auto& entity : *this)
			OnEntityUpdate(entity, info);
		OnPostEntityUpdate(EngineData, chain);
	}
}
