﻿#pragma once
#include "Array.h"
#include "VkMemBlock.h"

namespace jlb 
{
	class LinearAllocator;
}

namespace vk
{
	struct App;

	/// <summary>
	/// Linear allocator for allocations made with Vulkan. This includes GPU allocations.
	/// </summary>
	class LinearAllocator final
	{
	public:
		void Allocate(jlb::LinearAllocator& allocator, App& app);
		void Free(jlb::LinearAllocator& allocator, App& app);

		// Updates a pool to conform to the largest requested alignment.
		void RequestAlignment(VkDeviceSize size, uint32_t poolId);
		// Add to the size of the memory that will be allocated, based on the largest requested alignment.
		void Reserve(VkDeviceSize size, uint32_t poolId);
		// Allocates the memory pools based on the total size of all the reserve calls.
		void Compile(App& app);
		// Returns a block from the allocated memory.
		[[nodiscard]] MemBlock CreateBlock(VkDeviceSize size, uint32_t poolId);
		// Frees a block from the allocated memory. This is a linear allocator, so it assumes it is the last allocated block.
		void FreeBlock(MemBlock& block);

		// Get the ID of a pool that supports a certain memory type.
		[[nodiscard]] static uint32_t GetPoolId(App& app, uint32_t typeFilter, VkMemoryPropertyFlags properties);
		// Calculates the buffer size depending on the minimum offset.
		[[nodiscard]] static VkDeviceSize CalculateBufferSize(VkDeviceSize size, VkDeviceSize alignment);

	private:
		struct Pool final
		{
			VkDeviceMemory memory = VK_NULL_HANDLE;
			VkMemoryPropertyFlags memPropertyFlags;

			VkDeviceSize size;
			VkDeviceSize remaining;
			VkDeviceSize alignment;
		};

		jlb::Array<Pool> _pools{};
	};
}
