#pragma once

namespace vk::sync
{
	[[nodiscard]] VkFenceCreateInfo CreateFenceDefaultInfo();
	[[nodiscard]] VkSemaphoreCreateInfo CreateSemaphoreDefaultInfo();
}