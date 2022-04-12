#pragma once
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
		void AllocatePool(App& app, VkDeviceSize size, VkDeviceSize alignment, uint32_t poolId);
		// Returns a block from the allocated memory.
		[[nodiscard]] MemBlock CreateBlock(VkDeviceSize size, uint32_t poolId);
		// Frees a block from the allocated memory. This is a linear allocator, so it assumes it is the last allocated block.
		void FreeBlock(MemBlock& block);

		// Get the ID of a pool that supports a certain memory type.
		[[nodiscard]] static uint32_t GetPoolId(App& app, uint32_t typeFilter, VkMemoryPropertyFlags properties);
		// Calculates the buffer size depending on the minimum offset.
		[[nodiscard]] static VkDeviceSize CalculateBufferSize(VkDeviceSize size, VkDeviceSize alignment);

		void GetPoolInfo(uint32_t poolId, VkDeviceSize& outTotalRequestedSpace, VkDeviceSize& outLargestAlignment);
		[[nodiscard]] size_t GetLength() const;

	private:
		struct Pool final
		{
			VkDeviceMemory memory = VK_NULL_HANDLE;
			VkMemoryPropertyFlags memPropertyFlags;

			VkDeviceSize size = 0;
			VkDeviceSize remaining = 0;
			VkDeviceSize alignment;

			VkDeviceSize largestAlignmentRequested = 0;
		};

		jlb::Array<Pool> _pools{};
	};
}
