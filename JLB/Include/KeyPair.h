#pragma once
#include <cstdint>

namespace jlb
{
	// Basic structure which holds a key and a value.
	template <typename T>
	struct KeyPair final
	{
		T value{};
		size_t key = SIZE_MAX;
	};
}
