﻿#pragma once
#include "Array.h"
#include "VkMemBlock.h"

namespace jlb 
{
	class StackAllocator;
}

namespace vk
{
	struct App;

	/// <summary>
	/// Linear allocator for allocations made with Vulkan. This includes GPU allocations.
	/// </summary>
	class StackAllocator final
	{
	public:
		void Allocate(jlb::StackAllocator& allocator, App& app);
		void Free(jlb::StackAllocator& allocator, App& app);

		// Updates a pool to conform to the largest requested alignment.
		void DefinePool(VkDeviceSize size, VkDeviceSize alignment, uint32_t poolId);
		// Returns a block from the allocated memory.
		// If this is the first time allocating from this pool, it allocates the memory for the pool.
		[[nodiscard]] MemBlock CreateBlock(App& app, VkDeviceSize size, VkDeviceSize alignmentRequirement, uint32_t poolId);
		// Frees a block from the allocated memory. This is a linear allocator, so it assumes it is the last allocated block.
		void FreeBlock(const MemBlock& block);

		// Get the ID of a pool that supports a certain memory type.
		[[nodiscard]] static uint32_t GetPoolId(App& app, uint32_t typeFilter, VkMemoryPropertyFlags properties);
		// Calculates the buffer size depending on the minimum offset.
		[[nodiscard]] static VkDeviceSize CalculateBufferSize(VkDeviceSize size, VkDeviceSize alignment);
		// Returns information about the given pool. Useful when determining how much space the program occupies.
		void GetPoolInfo(uint32_t poolId, VkDeviceSize& outTotalRequestedSpace, VkDeviceSize& outLargestAlignment);
		[[nodiscard]] size_t GetLength() const;
		// Returns whether or not all the blocks have been properly freed.
		[[nodiscard]] bool IsEmpty();

	private:
		struct Pool final
		{
			VkDeviceMemory memory = VK_NULL_HANDLE;
			VkMemoryPropertyFlags memPropertyFlags;

			VkDeviceSize size = 0;
			VkDeviceSize remaining = 0;
			VkDeviceSize alignment;

			VkDeviceSize largestAlignmentRequested = 0;
			VkDeviceSize largestSpaceOccupied = 0;
			size_t allocId = 0;
		};

		jlb::Array<Pool> _pools{};
	};
}