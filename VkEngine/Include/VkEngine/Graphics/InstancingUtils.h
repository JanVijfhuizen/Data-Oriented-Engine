#pragma once
#include "Array.h"
#include "Buffer.h"
#include "VkRenderer/VkApp.h"
#include "VkRenderer/VkStackAllocator.h"
#include <VkEngine/EngineData.h>

namespace vke::instancing
{
	template <typename Type>
	[[nodiscard]] jlb::Array<Buffer> CreateStorageBuffers(const EngineData& info, size_t length);

	template <typename Type>
	jlb::Array<Buffer> CreateStorageBuffers(const EngineData& info, const size_t length)
	{
		auto& app = *info.app;
		auto& vkAllocator = *info.vkAllocator;
		auto& logicalDevice = app.logicalDevice;

		VkBufferCreateInfo vertBufferInfo{};
		vertBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertBufferInfo.size = sizeof(Type) * length;
		vertBufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		vertBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		const size_t swapChainImageCount = info.swapChainData->imageCount;

		jlb::Array<Buffer> buffers{};
		buffers.Allocate(*info.allocator, swapChainImageCount);

		for (size_t i = 0; i < swapChainImageCount; ++i)
		{
			auto& buffer = buffers[i].buffer;
			auto result = vkCreateBuffer(logicalDevice, &vertBufferInfo, nullptr, &buffer);
			assert(!result);

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

			const uint32_t poolId = vkAllocator.GetPoolId(app, memRequirements.memoryTypeBits,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			auto& memBlock = buffers[i].memBlock = vkAllocator.AllocateBlock(app, memRequirements.size, memRequirements.alignment, poolId);

			result = vkBindBufferMemory(logicalDevice, buffer, memBlock.memory, memBlock.offset);
			assert(!result);
		}

		return buffers;
	}
}
