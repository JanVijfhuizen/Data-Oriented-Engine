#pragma once
#include <cstdint>

namespace jlb
{
	// Identifier for memory allocations.
	struct AllocationID final
	{
		// Source pointer, used to validate on free.
		size_t id = SIZE_MAX;
		// Source pointer, used to validate on free.
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
