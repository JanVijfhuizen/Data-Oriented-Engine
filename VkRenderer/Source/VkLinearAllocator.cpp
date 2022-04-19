#include "pch.h"
#include "VkLinearAllocator.h"
#include "VkApp.h"
#include "JlbMath.h"

namespace vk
{
	void LinearAllocator::Allocate(jlb::LinearAllocator& allocator, App& app)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(app.physicalDevice, &memProperties);

		_pools.Allocate(allocator, memProperties.memoryTypeCount);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
		{
			auto& allocation = _pools[i];
			auto& memType = memProperties.memoryTypes[i];
			allocation.memPropertyFlags = memType.propertyFlags;
		}
	}

	void LinearAllocator::Free(jlb::LinearAllocator& allocator, App& app)
	{
		for (auto& pool : _pools)
		{
			if (pool.size == 0)
				continue;
			vkFreeMemory(app.logicalDevice, pool.memory, nullptr);
		}
		_pools.Free(allocator);
	}

	void LinearAllocator::AllocatePool(App& app, const VkDeviceSize size, const VkDeviceSize alignment, const uint32_t poolId)
	{
		if (size == 0)
			return;

		auto& pool = _pools[poolId];
		pool.alignment = alignment;
		pool.size = size;
		pool.remaining = size;

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = pool.size;
		allocInfo.memoryTypeIndex = poolId;

		const auto result = vkAllocateMemory(app.logicalDevice, &allocInfo, nullptr, &pool.memory);
		assert(!result);
	}

	MemBlock LinearAllocator::CreateBlock(const VkDeviceSize size, const uint32_t poolId)
	{
		auto& pool = _pools[poolId];
		const VkDeviceSize alignedSize = CalculateBufferSize(size, pool.alignment);
		const VkDeviceSize offset = pool.size - pool.remaining;
		assert(alignedSize <= pool.remaining);
		pool.remaining -= alignedSize;

		// Debugging purposes.
		pool.largestAlignmentRequested = jlb::Math::Max(pool.largestAlignmentRequested, alignedSize);
		pool.largestSpaceOccupied = jlb::Math::Max(pool.largestSpaceOccupied, pool.size - pool.remaining);

		MemBlock block{};
		block.memory = pool.memory;
		block.size = size;
		block.alignedSize = alignedSize;
		block.offset = offset;
		block.poolId = poolId;

		return block;
	}

	void LinearAllocator::FreeBlock(MemBlock& block)
	{
		auto& pool = _pools[block.poolId];
		pool.remaining += block.alignedSize;
	}

	uint32_t LinearAllocator::GetPoolId(App& app, const uint32_t typeFilter, const VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(app.physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			if (typeFilter & 1 << i)
			{
				auto& memType = memProperties.memoryTypes[i];
				const bool requiredPropertiesPresent = (memType.propertyFlags & properties) == properties;
				if (!requiredPropertiesPresent)
					continue;

				return i;
			}

		return UINT32_MAX;
	}

	VkDeviceSize LinearAllocator::CalculateBufferSize(const VkDeviceSize size, const VkDeviceSize alignment)
	{
		return (size / alignment + (size % alignment > 0)) * alignment;
	}

	void LinearAllocator::GetPoolInfo(const uint32_t poolId, 
		VkDeviceSize& outTotalRequestedSpace,
		VkDeviceSize& outLargestAlignment)
	{
		auto& pool = _pools[poolId];
		outTotalRequestedSpace = pool.largestSpaceOccupied;
		outLargestAlignment = pool.largestAlignmentRequested;
	}

	size_t LinearAllocator::GetLength() const
	{
		return _pools.GetLength();
	}

	bool LinearAllocator::IsEmpty()
	{
		for (auto& pool : _pools)
		{
			if (!pool.memory)
				continue;
			if (pool.remaining != pool.size)
				return false;
		}

		return true;
	}
}
