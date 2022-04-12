#pragma once

namespace vk
{
	class BufferHandler final
	{
	public:
		[[nodiscard]] static VkBufferCreateInfo CreateBufferDefaultInfo(VkDeviceSize size, VkBufferUsageFlags flags);
	};
}