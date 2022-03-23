#pragma once

namespace vk
{
	struct App final
	{
		VkDebugUtilsMessengerEXT debugger;
		VkInstance instance;
		VkSurfaceKHR surface;
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
		VkCommandPool commandPool;
	};
}