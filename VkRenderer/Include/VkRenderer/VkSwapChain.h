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

	constexpr size_t SWAPCHAIN_MAX_FRAMES_IN_FLIGHT = 4;

	/// <summary>
	/// Handles rendering to the screen.
	/// </summary>
	class SwapChain final
	{
	public:
		// Window background color.
		VkClearValue clearColor = { 0, 0, 0, 1 };

		void Allocate(jlb::LinearAllocator& allocator, App& app, IWindowHandler& windowHandler);
		void Free(jlb::LinearAllocator& allocator, App& app);

		/// <summary>
		/// Begins a new command and render pass.
		/// </summary>
		/// <returns>Command buffer used for the render pass.</returns>
		[[nodiscard]] VkCommandBuffer BeginFrame(App& app);
		/// <summary>
		/// Ends the render pass and presents it to the screen.
		/// </summary>
		/// <param name="waitSemaphores">Additional semaphores to wait for.</param>
		/// <returns>Present result.</returns>
		[[nodiscard]] VkResult EndFrame(jlb::LinearAllocator& tempAllocator, App& app, jlb::ArrayView<VkSemaphore> waitSemaphores = {});
		/// <summary>
		/// Recreates the swap chain. Call this if EndFrame returns something else than VK_SUCCESS.
		/// </summary>
		void Recreate(jlb::LinearAllocator& tempAllocator, App& app, IWindowHandler& windowHandler);
		/// <summary>
		/// Returns the render pass used to draw to the screen.
		/// </summary>
		[[nodiscard]] VkRenderPass GetRenderPass() const;
		[[nodiscard]] glm::ivec2 GetResolution() const;

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

		void WaitForImage(App& app);
		void Cleanup(App& app);

		// Choose color formatting for the swap chain images, like RGB(A).
		[[nodiscard]] static VkSurfaceFormatKHR ChooseSurfaceFormat(jlb::Array<VkSurfaceFormatKHR>& availableFormats);
		// Choose the way the swapchain presents the images.
		[[nodiscard]] static VkPresentModeKHR ChoosePresentMode(jlb::Array<VkPresentModeKHR>& availablePresentModes);
		// Choose the resolution for the swap chain images.
		[[nodiscard]] static VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, glm::ivec2 resolution);
	};
}
