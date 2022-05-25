#include "pch.h"
#include "Graphics/Buffer.h"
#include "VkRenderer/VkApp.h"
#include "VkRenderer/VkStackAllocator.h"

namespace game
{
	void FreeBuffer(const EngineOutData& outData, Buffer& buffer)
	{
		vkDestroyBuffer(outData.app->logicalDevice, buffer.buffer, nullptr);
		outData.vkAllocator->FreeBlock(buffer.memBlock);
	}
}
