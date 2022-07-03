#include "VkEngine/pch.h"
#include "VkRenderer/VkApp.h"
#include "VkRenderer/VkStackAllocator.h"
#include "VkEngine/Graphics/Buffer.h"

namespace vke::buffer
{
	void Free(const EngineData& info, const Buffer& buffer)
	{
		vkDestroyBuffer(info.app->logicalDevice, buffer.buffer, nullptr);
		info.vkAllocator->FreeBlock(buffer.memBlock);
	}
}
