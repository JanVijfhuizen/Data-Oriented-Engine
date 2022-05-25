#include "pch.h"
#include "VkBufferUtils.h"

namespace vk::buffer
{
	VkBufferCreateInfo CreateDefaultInfo(const VkDeviceSize size, const VkBufferUsageFlags flags)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = flags;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		return bufferInfo;
	}
}
