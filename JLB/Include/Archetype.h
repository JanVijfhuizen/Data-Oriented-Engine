#pragma once
#include "Vector.h"
#include "Tuple.h"

namespace jlb
{
	template <typename ...Args>
	class Archetype : public jlb::Vector<jlb::Tuple<Args...>>
	{
	public:
		using Entity = jlb::Tuple<Args...>;
	};
}
