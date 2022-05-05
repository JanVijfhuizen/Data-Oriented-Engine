#pragma once
#include "Vector.h"

namespace jlb
{
	/// <summary>
	/// Class used to store and specify a type of entities.
	/// </summary>
	template <typename Entity, typename Info>
	class Archetype : public Vector<Entity>
	{
	public:
		// Define the resources required from the systems.
		virtual void DefineResourceUsage(Info& info) = 0;
		// Updates all entities in this archetype.
		void Update(Info& info);

	protected:
		// Update a single entity.
		virtual void OnUpdate(Entity& entity, Info& info) = 0;

	private:
		using Vector<Entity>::AllocateAndCopy;
	};

	template <typename Entity, typename Info>
	void Archetype<Entity, Info>::Update(Info& info)
	{
		for (auto& entity : *this)
			OnUpdate(entity, info);
	}
}
