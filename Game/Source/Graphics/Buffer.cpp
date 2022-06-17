#include "pch.h"
#include "Graphics/Buffer.h"
#include "VkRenderer/VkApp.h"
#include "VkRenderer/VkStackAllocator.h"

namespace game
{
	void FreeBuffer(const EngineData& EngineData, Buffer& buffer)
	{
		vkDestroyBuffer(EngineData.app->logicalDevice, buffer.buffer, nullptr);
		EngineData.vkAllocator->FreeBlock(buffer.memBlock);
	}
}
