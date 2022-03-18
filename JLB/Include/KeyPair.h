#pragma once
#include <cstdint>

namespace jlb
{
	/// <summary>
	/// Basic structure to hold both a key and a value.
	/// </summary>
	template <typename T>
	struct KeyPair final
	{
		T value{};
		size_t key = SIZE_MAX;
	};
}
