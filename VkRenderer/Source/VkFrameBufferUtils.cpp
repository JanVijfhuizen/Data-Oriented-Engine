#include "pch.h"
#include "VkFrameBufferUtils.h"

namespace vk::frameBuffer
{
	VkFramebufferCreateInfo CreateDefaultInfo()
	{
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.layers = 1;
		return framebufferInfo;
	}
}
