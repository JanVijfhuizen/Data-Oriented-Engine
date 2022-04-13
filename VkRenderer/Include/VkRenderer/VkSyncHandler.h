#pragma once

namespace vk
{
	class SyncHandler final
	{
	public:
		[[nodiscard]] static VkFenceCreateInfo CreateFenceDefaultInfo();
		[[nodiscard]] static VkSemaphoreCreateInfo CreateSemaphoreDefaultInfo();
	};
}