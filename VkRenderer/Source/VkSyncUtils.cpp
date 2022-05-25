#include "pch.h"
#include "VkSyncUtils.h"

namespace vk::sync
{
	VkFenceCreateInfo CreateFenceDefaultInfo()
	{
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		return fenceInfo;
	}

	VkSemaphoreCreateInfo CreateSemaphoreDefaultInfo()
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		return semaphoreInfo;
	}
}
