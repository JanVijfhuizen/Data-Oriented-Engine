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
		void Allocate(jlb::LinearAllocator& allocator, App& app, IWindowHandler& windowHandler);
		void Free(jlb::LinearAllocator& allocator, App& app);

		void Recreate(jlb::LinearAllocator& tempAllocator, App& app, IWindowHandler& windowHandler);

	private:
		struct Image final
		{
			VkFence fence = VK_NULL_HANDLE;
		};

		struct Frame final
		{
			
		};

		VkSurfaceFormatKHR _surfaceFormat;
		VkPresentModeKHR _presentMode;

		VkExtent2D _extent;
		VkSwapchainKHR _swapChain = VK_NULL_HANDLE;

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
