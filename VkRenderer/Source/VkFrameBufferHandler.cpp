#include "pch.h"
#include "VkFrameBufferHandler.h"

namespace vk
{
	VkFramebufferCreateInfo FrameBufferHandler::CreateDefaultInfo()
	{
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.layers = 1;
		return framebufferInfo;
	}
}
