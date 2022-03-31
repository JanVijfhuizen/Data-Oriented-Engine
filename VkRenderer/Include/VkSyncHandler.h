#pragma once

namespace vk
{
	class FenceHandler final
	{
	public:
		[[nodiscard]] static VkFenceCreateInfo CreateDefaultInfo();
	};
}