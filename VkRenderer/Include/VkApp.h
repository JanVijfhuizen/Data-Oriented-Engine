#pragma once
#include <vulkan/vulkan.h>

namespace vk
{
	struct App final
	{
		struct Queues final
		{
			union
			{
				struct
				{
					VkQueue graphics;
					VkQueue present;
				};
				VkQueue values[2];
			};
		};

		VkDebugUtilsMessengerEXT debugger;
		VkInstance instance;
		VkSurfaceKHR surface;
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
		Queues queues;
		VkCommandPool commandPool;
	};
}
