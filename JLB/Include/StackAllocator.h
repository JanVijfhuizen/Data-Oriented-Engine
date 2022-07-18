#pragma once
#include <cassert>
#include <new.h>
#include "Allocation.h"

namespace jlb
{
	// Memory allocator that follows a last-in-first-out policy to optimize memory usage.
	class StackAllocator final
	{
	public:
		struct CreateInfo final
		{
			// Default size of a memory block.
			size_t pageSize = 4096;
			// You can have the stack allocator manage an already existing block of memory.
			void* data = nullptr;
		};

		void Allocate(const CreateInfo& info = {});
		void Free();

		// Manually allocate memory of a given size.
		[[nodiscard]] Allocation<void> Malloc(size_t size);
		// Free the allocation, provided that it was the last allocation made.<br>
		// Does not call destructors.
		void MFree(const AllocationID& allocation);

		// Frees the last allocation at the given source, but does not do any additional safety checks.
		void MFreeUnsafe(size_t* src);

		// Malloc but simplified for allocating types.
		template <typename T, typename ...Args>
		[[nodiscard]] Allocation<T> New(size_t count = 1, Args... args);
		[[nodiscard]] bool IsEmpty() const;
		// Get the amount of sub allocators made during it's runtime.
		[[nodiscard]] size_t GetDepth() const;
		// Check if this allocation is on top of one of the (sub) stack allocators.
		[[nodiscard]] bool IsOnTop(const AllocationID& allocation) const;
		// After calling this, the allocator will be considered empty. Nesting allocators will still remain.
		void Clear();

	private:
		StackAllocator* _next = nullptr;
		size_t* _data = nullptr;
		size_t _size{};
		size_t _current = 0;
		size_t _id = 0;

		[[nodiscard]] static size_t ToChunkSize(size_t size);
	};

	template <typename T, typename ...Args>
	Allocation<T> StackAllocator::New(const size_t count, Args... args)
	{
		const auto alloc = Malloc(sizeof(T) * count);
		Allocation<T> allocation{};
		allocation.ptr = reinterpret_cast<T*>(alloc.ptr);
		allocation.id = alloc.id;

		for (size_t i = 0; i < count; ++i)
			new(&allocation.ptr[i]) T(args...);
		return allocation;
	}
}
