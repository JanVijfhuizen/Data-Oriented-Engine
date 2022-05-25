#pragma once

namespace vk::buffer
{
	[[nodiscard]] static VkBufferCreateInfo CreateDefaultInfo(VkDeviceSize size, VkBufferUsageFlags flags);
}