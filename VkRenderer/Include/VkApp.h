#pragma once

namespace vk
{
	/// <summary>
	/// Contains everything you need to run a vulkan application.<br>
	/// Can be generated with the VkBootstrap.
	/// </summary>
	struct App final
	{
		/// <summary>
		/// Contains the various graphic queues used to push commands to.
		/// </summary>
		struct Queues final
		{
			union
			{
				struct
				{
					// Queue for graphics operations.
					VkQueue graphics;
					// Queue for presentation operations (rendering to the screen).
					VkQueue present;
				};
				VkQueue values[2];
			};
		};

		// Debugger that logs to the command prompt.
		VkDebugUtilsMessengerEXT debugger;
		// Vulkan application instance.
		VkInstance instance;
		// Window surface.
		VkSurfaceKHR surface;
		// GPU Hardware used for rendering.
		VkPhysicalDevice physicalDevice;
		// Interface to GPU hardware.
		VkDevice logicalDevice;
		// Render queues for graphics operations.
		Queues queues;
		// Pool for creating commands.
		VkCommandPool commandPool;
	};
}
