#pragma once

namespace jlb
{
	class StackAllocator;
}

namespace vk
{
	struct App;
	class StackAllocator;
}

namespace vke
{
	/// <summary>
	/// Swap chain data passed from the engine to the game systems.<br>
	/// </summary>
	struct EngineSwapChainData final
	{
		// Command buffer that is currently recording.
		VkCommandBuffer commandBuffer;
		// Render pass used by the swap chain.
		VkRenderPass renderPass;
		// Resolution of the swap chain.
		glm::ivec2 resolution;
		// Image index for the active swap chain image.
		uint8_t imageIndex;
		// Amount of swap chain images.
		uint8_t imageCount;
	};

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
		// All swap chain related data.
		EngineSwapChainData* swapChainData;

		// Manages all the engine's systems.
		jlb::Systems<EngineData> systems;

		// Amount of time passed since the start of the game. In milliseconds.
		float time = 0;
		// Duration of the previous frame. In milliseconds.
		float deltaTime = 0;
		// Mouse position.
		glm::vec2 mousePos;
		// Whether or not the mouse is locked to the screen.
		bool mouseAvailable = true;
	};
}