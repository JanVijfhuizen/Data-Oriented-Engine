#pragma once

namespace vk
{
	class FrameBufferHandler final
	{
	public:
		[[nodiscard]] static VkFramebufferCreateInfo CreateDefaultInfo();
	};
}