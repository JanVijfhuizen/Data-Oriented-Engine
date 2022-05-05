#include "pch.h"
#include "VkStackAllocator.h"
#include "VkApp.h"
#include "JlbMath.h"
#include "VkMemBlock.h"

namespace vk
{
	void StackAllocator::Allocate(App& app, const size_t pageSize)
	{
		_pageSize = pageSize;
		_cpuAllocator.Allocate();

		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(app.physicalDevice, &memProperties);
		_rootPools.Allocate(_cpuAllocator, memProperties.memoryTypeCount);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
		{
			auto& pool = _rootPools[i];
			auto& memType = memProperties.memoryTypes[i];
			pool.memPropertyFlags = memType.propertyFlags;
		}
	}

	void StackAllocator::Free(App& app)
	{
		for (auto& pool : _rootPools)
		{
			auto current = &pool;
			while(current)
			{
				if(current->memory != VK_NULL_HANDLE)
					vkFreeMemory(app.logicalDevice, pool.memory, nullptr);
				current = current->next;
			}	
		}

		// Don't have to clean up individual cpu allocations.
		_cpuAllocator.Free();
	}

	MemBlock StackAllocator::AllocateBlock(App& app, const VkDeviceSize size, 
		const VkDeviceSize alignment, const uint32_t poolId)
	{
		// Try and find a valid sub pool.
		Pool* current = &_rootPools[poolId];
		while(current)
		{
			if (current->remaining >= size && current->alignment == alignment)
				break;

			current = current->next;
		}

		// If it does not exist yet, create one.
		if(!current)
		{
			const auto pool = _cpuAllocator.New<Pool>();
			InitPool(*pool.ptr, app, size, alignment, poolId);

			// Find the last in the pool chain and link it.
			current = &_rootPools[poolId];
			while(true)
			{
				if (!current->next)
					break;
				current = current->next;
			}

			current->next = pool.ptr;
			current = pool.ptr;
		}

		// Now that we have the actual correct memory pool, allocate the block.
		const VkDeviceSize alignedSize = CalculateBufferSize(size, current->alignment);
		const VkDeviceSize offset = current->size - current->remaining;
		current->remaining -= alignedSize;

		MemBlock block{};
		block.memory = current->memory;
		block.size = size;
		block.alignedSize = alignedSize;
		block.offset = offset;
		block.poolId = poolId;
		block.allocId = current->allocId++;
		return block;
	}

	void StackAllocator::FreeBlock(const MemBlock& memBlock)
	{
		Pool* current = &_rootPools[memBlock.poolId];
		while (current)
		{
			if (current->memory == memBlock.memory)
				break;
			current = current->next;
		}
		assert(current);

		assert(current->allocId == memBlock.allocId - 1);
		--current->allocId;
		current->remaining += memBlock.alignedSize;
	}

	uint32_t StackAllocator::GetPoolId(App& app, const uint32_t typeFilter, const VkMemoryPropertyFlags properties)
	{
		int32_t i = -1;
		for (auto& pool : _rootPools)
		{
			++i;

			if(typeFilter & 1 << i)
			{
				const bool requiredPropertiesPresent = (pool.memPropertyFlags & properties) == properties;
				if (!requiredPropertiesPresent)
					continue;

				return i;
			}	
		}

		return UINT32_MAX;
	}

	bool StackAllocator::IsEmpty()
	{
		for (auto& pool : _rootPools)
		{
			auto current = &pool;
			while(current)
			{
				if (current->size != current->remaining)
					return false;
				current = current->next;
			}
		}
		return true;
	}

	void StackAllocator::InitPool(Pool& pool, App& app, const VkDeviceSize size, 
		const VkDeviceSize alignment, const uint32_t poolId) const
	{
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = jlb::Math::Max(size, _pageSize);
		allocInfo.memoryTypeIndex = poolId;

		const auto result = vkAllocateMemory(app.logicalDevice, &allocInfo, nullptr, &pool.memory);
		assert(!result);

		pool.size = allocInfo.allocationSize;
		pool.alignment = alignment;
		pool.remaining = pool.size;
	}

	VkDeviceSize StackAllocator::CalculateBufferSize(const VkDeviceSize size, const VkDeviceSize alignment)
	{
		return (size / alignment + (size % alignment > 0)) * alignment;
	}
}
