#pragma once
#include "Array.h"

namespace jlb 
{
	class LinearAllocator;
}

namespace vk
{
	struct App;
	class IWindowHandler;

	constexpr size_t SWAPCHAIN_MAX_FRAMES_IN_FLIGHT = 3;

	class SwapChain final
	{
	public:
		VkClearValue clearColor = { 0, 0, 0, 1 };

		void Allocate(jlb::LinearAllocator& allocator, App& app);
		void Free(jlb::LinearAllocator& allocator, App& app);

		void WaitForImage(App& app);
		[[nodiscard]] VkCommandBuffer BeginRenderPass();
		[[nodiscard]] VkResult EndRenderPassAndPresent(jlb::LinearAllocator& tempAllocator, App& app, jlb::ArrayView<VkSemaphore> waitSemaphores = {});

		void Recreate(jlb::LinearAllocator& tempAllocator, App& app, IWindowHandler& windowHandler);

	private:
		struct Image final
		{
			VkImage colorImage;
			VkImageView colorImageView;
			VkCommandBuffer cmdBuffer;
			VkFramebuffer frameBuffer;
			VkFence inFlightFence = VK_NULL_HANDLE;
		};

		struct Frame final
		{
			VkSemaphore imageAvailableSemaphore;
			VkSemaphore renderFinishedSemaphore;
			VkFence inFlightFence;
		};

		VkSurfaceFormatKHR _surfaceFormat;
		VkPresentModeKHR _presentMode;

		VkExtent2D _extent;
		VkSwapchainKHR _swapChain = VK_NULL_HANDLE;
		VkRenderPass _renderPass;

		uint32_t _imageIndex = 0;
		uint8_t _frameIndex = 0;

		jlb::Array<Image> _images{};
		jlb::Array<Frame> _frames{};

		void Cleanup(App& app);

		// Choose color formatting for the swap chain images, like RGB(A).
		[[nodiscard]] static VkSurfaceFormatKHR ChooseSurfaceFormat(jlb::Array<VkSurfaceFormatKHR>& availableFormats);
		// Choose the way the swapchain presents the images.
		[[nodiscard]] static VkPresentModeKHR ChoosePresentMode(jlb::Array<VkPresentModeKHR>& availablePresentModes);
		// Choose the resolution for the swap chain images.
		[[nodiscard]] static VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, glm::ivec2 resolution);
	};
}
