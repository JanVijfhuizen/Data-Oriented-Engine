#pragma once
#include "SystemManager.h"

namespace jlb
{
	class StackAllocator;
}

namespace vk
{
	struct App;
	class StackAllocator;
}

namespace game
{
	/// <summary>
	/// Data passed from the engine to the game systems.<br>
	/// </summary>
	struct EngineData final
	{
		// Used for mostly permanent allocations.
		jlb::StackAllocator* allocator;
		// Used for temporary allocations.
		jlb::StackAllocator* tempAllocator;
		// Used for Vulkan allocations.
		vk::StackAllocator* vkAllocator;
		// App containing all relevant Vulkan data.
		vk::App* app;

		// Command buffer that is currently recording.
		VkCommandBuffer swapChainCommandBuffer;
		// Render pass used by the swap chain.
		VkRenderPass swapChainRenderPass;
		// Resolution of the application.
		glm::ivec2 resolution;
		// Image index for the active Vulkan swapchain image.
		uint8_t swapChainImageIndex;
		// Amount of Vulkan swapchain images.
		uint8_t swapChainImageCount;

		// Manages all the engine's systems.
		jlb::Systems<EngineData> systems;

		// Amount of time passed since the start of the game. In milliseconds.
		float time = 0;
		// Duration of the previous frame. In milliseconds.
		float deltaTime = 0;
		// Mouse position.
		glm::vec2 mousePos;
	};
}