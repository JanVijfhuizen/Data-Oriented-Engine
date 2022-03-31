#include "pch.h"
#include "VkSyncHandler.h"

namespace vk
{
	VkFenceCreateInfo FenceHandler::CreateDefaultInfo()
	{
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		return fenceInfo;
	}
}
