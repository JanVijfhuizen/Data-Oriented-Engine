#pragma once
#include "ArrayView.h"

namespace jlb
{
	class LinearAllocator;
}

namespace vk
{
	struct App;
	class LinearAllocator;
}

namespace game
{
	struct EngineOutData final
	{
		jlb::LinearAllocator* allocator;
		jlb::LinearAllocator* tempAllocator;
		vk::LinearAllocator* vkAllocator;
		vk::App* app;

		VkCommandBuffer swapChainCommandBuffer;
		VkRenderPass swapChainRenderPass;
		glm::ivec2 resolution;
		uint8_t swapChainImageIndex;
		uint8_t swapChainImageCount;

		// In milliseconds.
		float time = 0;
		// In milliseconds.
		float deltaTime = 0;
	};

	struct EngineInData final
	{
		jlb::ArrayView<VkSemaphore> swapChainWaitSemaphores{};
	};
}