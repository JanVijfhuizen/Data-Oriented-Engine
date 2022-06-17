#pragma once
#include <cstdint>

namespace jlb
{
	// Identifier for memory allocations.
	struct AllocationID final
	{
		size_t index = SIZE_MAX;
		size_t* src = nullptr;

		[[nodiscard]] operator bool() const;
	};

	// Allocated block of memory with the stack allocator.
	template <typename T>
	struct Allocation final
	{
		T* ptr = nullptr;
		AllocationID id{};
	};
}
