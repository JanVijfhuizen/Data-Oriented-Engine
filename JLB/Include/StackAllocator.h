﻿#pragma once
#include <cassert>

namespace jlb
{
	/// <summary>
	/// Memory allocator that follows a last-in-first-out policy to optimize memory usage.
	/// </summary>
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

		// Allocated block of memory with the stack allocator.
		template <typename T>
		struct Allocation final
		{
			// Allocated data.
			T* ptr = nullptr;
			// Source pointer, used to validate on free.
			size_t id;
			// Source pointer, used to validate on free.
			size_t* src;

			[[nodiscard]] operator bool() const;
		};

		void Allocate(const CreateInfo& info = {});
		void Free();

		// Manually allocate memory of a given size.
		[[nodiscard]] Allocation<void> Malloc(size_t size);
		// Free the allocation, provided that it was the last allocation made.<br>
		// Does not call destructors.
		template <typename T>
		void MFree(const Allocation<T>& allocation);

		// Frees the last allocation at the given source, but does not do any additional safety checks.
		void MFreeUnsafe(size_t* src);

		// Malloc but simplified for allocating types. Does not call constructors.
		template <typename T>
		[[nodiscard]] Allocation<T> New(size_t count = 1);
		[[nodiscard]] bool IsEmpty() const;
		// Get the amount of sub allocators made during it's runtime.
		[[nodiscard]] size_t GetDepth() const;

	private:
		StackAllocator* _next = nullptr;
		size_t* _data = nullptr;
		size_t _size{};
		size_t _current = 0;
		size_t _id = 0;

		[[nodiscard]] static size_t ToChunkSize(size_t size);
	};

	template <typename T>
	StackAllocator::Allocation<T>::operator bool() const
	{
		return ptr;
	}

	template <typename T>
	void StackAllocator::MFree(const Allocation<T>& allocation)
	{
		if (_data != allocation.src)
			return _next->MFree(allocation);
		assert(_id == allocation.id + 1);

		MFreeUnsafe(allocation.src);
	}

	template <typename T>
	StackAllocator::Allocation<T> StackAllocator::New(const size_t count)
	{
		auto alloc = Malloc(sizeof(T) * count);
		Allocation<T> allocation{};
		allocation.id = alloc.id;
		allocation.ptr = reinterpret_cast<T*>(alloc.ptr);
		allocation.src = alloc.src;

		for (size_t i = 0; i < count; ++i)
			new(&allocation.ptr[i]) T();
		return allocation;
	}
}
