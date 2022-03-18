#pragma once

namespace vk
{
	struct App final
	{
		VkDebugUtilsMessengerEXT debugger;
		VkInstance instance;
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
		VkCommandPool commandPool;
	};
}