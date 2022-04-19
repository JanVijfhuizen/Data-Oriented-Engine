#pragma once
#include "Vector.h"
#include "Tuple.h"

namespace jlb
{
	template <typename ComponentTuple, typename ...Systems>
	class Archetype;

	template <typename ...Components, typename ...Systems>
	class Archetype<Tuple<Components...>, Systems...> : public Vector<Tuple<Components...>>
	{
	public:
		using Entity = Tuple<Components...>;

		virtual void DefineResourceUsage(Systems&...) = 0;
		void Update(Systems&... systems);

	protected:
		virtual void OnUpdate(Entity& entity, Systems&... systems) = 0;
	};

	template <typename ... Components, typename ... Systems>
	void Archetype<tupleImpl::TupleImpl<0, Components...>, Systems...>::Update(Systems&... systems)
	{
		for (auto& entity : *this)
			OnUpdate(entity, systems...);
	}
}
