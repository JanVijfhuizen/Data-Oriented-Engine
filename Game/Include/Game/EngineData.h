#pragma once
#include "ArrayView.h"

namespace jlb
{
	class StackAllocator;
}

namespace vk
{
	struct App;
	class LinearAllocator;
}

namespace game
{
#ifndef MEM_DEFAULT_SIZE
	// During the initial engine test runs, this will be used as the default memory size for the linear allocators.
#define MEM_DEFAULT_SIZE 65536  // NOLINT(cppcoreguidelines-macro-usage)
#endif
#ifndef VK_MEM_DEFAULT_SIZE
// During the initial engine test runs, this will be used as the default memory size for the Vulkan memory pools.
#define VK_MEM_DEFAULT_SIZE 262144  // NOLINT(cppcoreguidelines-macro-usage)
#endif
#ifndef VK_MEM_DEFAULT_ALIGNMENT
// During the initial engine test runs, this will be used as the default memory alignment for Vulkan memory pools.
#define VK_MEM_DEFAULT_ALIGNMENT 1024  // NOLINT(cppcoreguidelines-macro-usage)
#endif

	/// <summary>
	/// Data passed from the engine to the game framework.<br>
	/// One very important thing to note is that the allocators REQUIRE you to be very stable with memory allocations.<br>
	/// The engine is going to allocate memory based on how much you use during the first frame.<br>
	/// Allocating more than that will result in a crash. If you want to be more flexible with your memory, you can always allocate the traditional way.
	/// </summary>
	struct EngineOutData final
	{
		// Used for mostly permanent allocations.
		jlb::StackAllocator* allocator;
		// Used for temporary allocations.
		jlb::StackAllocator* tempAllocator;
		// Used for Vulkan allocations.
		vk::LinearAllocator* vkAllocator;
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

		// Amount of time passed since the start of the game. In milliseconds.
		float time = 0;
		// Duration of the previous frame. In milliseconds.
		float deltaTime = 0;
	};

	/// <summary>
	/// Data passed from the game into the engine.
	/// </summary>
	struct EngineInData final
	{
		// The swapchain will wait before submitting until these semaphores are done.
		jlb::ArrayView<VkSemaphore> swapChainWaitSemaphores{};
	};
}