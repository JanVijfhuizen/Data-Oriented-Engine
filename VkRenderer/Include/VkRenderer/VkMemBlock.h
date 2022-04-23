#pragma once

namespace vk
{
	/// <summary>
	/// Block of vulkan memory, allocated with the linear allocator.
	/// </summary>
	struct MemBlock final
	{
		VkDeviceMemory memory;
		VkDeviceSize size;
		VkDeviceSize alignedSize;
		VkDeviceSize offset;
		uint32_t poolId;
		size_t allocId;
	};
}