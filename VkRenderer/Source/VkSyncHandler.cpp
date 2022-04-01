#include "pch.h"
#include "VkSyncHandler.h"

namespace vk
{
	VkFenceCreateInfo SyncHandler::CreateFenceDefaultInfo()
	{
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		return fenceInfo;
	}

	VkSemaphoreCreateInfo SyncHandler::CreateSemaphoreDefaultInfo()
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		return semaphoreInfo;
	}
}
