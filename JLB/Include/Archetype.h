#pragma once
#include "Vector.h"
#include "Tuple.h"

namespace jlb
{
	template <typename ...Args>
	class Archetype : public Vector<Tuple<Args...>>
	{
	public:
		using Entity = Tuple<Args...>;
	};
}
