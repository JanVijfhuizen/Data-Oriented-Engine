#pragma once

namespace vk
{
	class ImageHandler final
	{
	public:
		[[nodiscard]] static VkImageViewCreateInfo CreateViewDefaultInfo();
	};
}
