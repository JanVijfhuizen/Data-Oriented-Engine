#pragma once
#include "Vector.h"
#include "Tuple.h"

namespace jlb
{
	/// <summary>
	/// Entity Component Archetype. Used to define a type of entity and the components that it uses.
	/// </summary>
	/// <typeparam name="ComponentTuple">The components used by the entity.</typeparam>
	/// <typeparam name="...Systems">The systems used by the entity.</typeparam>
	template <typename ComponentTuple, typename ...Systems>
	class Archetype;

	template <typename ...Components, typename ...Systems>
	class Archetype<Tuple<Components...>, Systems...> : public Vector<Tuple<Components...>>
	{
	public:
		using Entity = Tuple<Components...>;

		// Define the resources required from the systems.
		virtual void DefineResourceUsage(Systems&...) = 0;
		// Updates all entities in this archetype.
		void Update(Systems&... systems);

	protected:
		// Update a single entity.
		virtual void OnUpdate(Entity& entity, Systems&... systems) = 0;
	};

	template <typename ... Components, typename ... Systems>
	void Archetype<tupleImpl::TupleImpl<0, Components...>, Systems...>::Update(Systems&... systems)
	{
		for (auto& entity : *this)
			OnUpdate(entity, systems...);
	}
}
