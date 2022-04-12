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

	void LinearAllocator::RequestAlignment(const VkDeviceSize size, const uint32_t poolId)
	{
		auto& pool = _pools[poolId];
		pool.alignment = jlb::Math::Max(size, pool.alignment);
	}

	void LinearAllocator::Compile(App& app)
	{
		uint32_t i = 0;
		for (auto& pool : _pools)
		{
			pool.remaining = pool.size;
			if (pool.size == 0)
				continue;

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = pool.size;
			allocInfo.memoryTypeIndex = i++;

			const auto result = vkAllocateMemory(app.logicalDevice, &allocInfo, nullptr, &pool.memory);
			assert(!result);
		}
	}

	MemBlock LinearAllocator::CreateBlock(const VkDeviceSize size, const uint32_t poolId)
	{
		auto& pool = _pools[poolId];
		const VkDeviceSize alignedSize = CalculateBufferSize(size, pool.alignment);
		const VkDeviceSize offset = pool.size - pool.remaining;
		assert(alignedSize <= pool.remaining);
		pool.remaining -= alignedSize;

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
		pool.remaining += block.size;
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

	void LinearAllocator::Reserve(const VkDeviceSize size, const uint32_t poolId)
	{
		auto& pool = _pools[poolId];
		assert(pool.size > 0);
		pool.size += CalculateBufferSize(size, pool.alignment);
	}

	VkDeviceSize LinearAllocator::CalculateBufferSize(const VkDeviceSize size, const VkDeviceSize alignment)
	{
		return (size / alignment + (size % alignment > 0)) * alignment;
	}
}
