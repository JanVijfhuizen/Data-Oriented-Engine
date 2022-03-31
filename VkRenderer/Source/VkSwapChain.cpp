#include "pch.h"
#include "VkSwapChain.h"
#include "VkBootstrap.h"
#include "VkApp.h"
#include "JlbMath.h"
#include "VkImageHandler.h"
#include "VkSyncHandler.h"
#include "VkCommandHandler.h"

namespace vk
{
	void SwapChain::Allocate(jlb::LinearAllocator& allocator, App& app, IWindowHandler& windowHandler)
	{
		auto support = Bootstrap::QuerySwapChainSupport(allocator, app);
		const uint32_t imageCount = support.GetRecommendedImageCount();
		_surfaceFormat = ChooseSurfaceFormat(support.formats);
		_presentMode = ChoosePresentMode(support.presentModes);
		support.Free(allocator);

		_images.Allocate(allocator, imageCount);
		_frames.Allocate(allocator, SWAPCHAIN_MAX_FRAMES_IN_FLIGHT);

		Recreate(allocator, app, windowHandler);
	}

	void SwapChain::Free(jlb::LinearAllocator& allocator, App& app)
	{
		Cleanup(app);

		_frames.Free(allocator);
		_images.Free(allocator);
	}

	void SwapChain::Cleanup(App& app)
	{
		if (!_swapChain)
			return;

		const auto result = vkDeviceWaitIdle(app.logicalDevice);
		assert(!result);

		for (auto& image : _images)
		{
			image.fence = VK_NULL_HANDLE;
			vkDestroyImageView(app.logicalDevice, image.colorImageView, nullptr);
			if (image.fence)
				vkWaitForFences(app.logicalDevice, 1, &image.fence, VK_TRUE, UINT64_MAX);
			vkFreeCommandBuffers(app.logicalDevice, app.commandPool, 1, &image.cmdBuffer);
		}

		for (auto& frame : _frames)
		{
			vkDestroySemaphore(app.logicalDevice, frame.imageAvailableSemaphore, nullptr);
			vkDestroySemaphore(app.logicalDevice, frame.renderFinishedSemaphore, nullptr);
			vkDestroyFence(app.logicalDevice, frame.inFlightFence, nullptr);
		}

		vkDestroySwapchainKHR(app.logicalDevice, _swapChain, nullptr);
	}

	VkSurfaceFormatKHR SwapChain::ChooseSurfaceFormat(jlb::Array<VkSurfaceFormatKHR>& availableFormats)
	{
		// Preferably go for SRGB, if it's not present just go with the first one found.
		// We can basically assume that SRGB is supported on most hardware.
		for (const auto& availableFormat : availableFormats)
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;
		return availableFormats[0];
	}

	VkPresentModeKHR SwapChain::ChoosePresentMode(jlb::Array<VkPresentModeKHR>& availablePresentModes)
	{
		// Preferably go for Mailbox, otherwise go for Fifo.
		// Fifo is traditional VSync, where mailbox is all that and better, but unlike Fifo is not required to be supported by the hardware.
		for (const auto& availablePresentMode : availablePresentModes)
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return availablePresentMode;
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D SwapChain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, const glm::ivec2 resolution)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
			return capabilities.currentExtent;

		VkExtent2D actualExtent =
		{
			static_cast<uint32_t>(resolution.x),
			static_cast<uint32_t>(resolution.y)
		};

		const auto& minExtent = capabilities.minImageExtent;
		const auto& maxExtent = capabilities.maxImageExtent;

		actualExtent.width = jlb::Math::Clamp(actualExtent.width, minExtent.width, maxExtent.width);
		actualExtent.height = jlb::Math::Clamp(actualExtent.height, minExtent.height, maxExtent.height);

		return actualExtent;
	}

	void SwapChain::Recreate(jlb::LinearAllocator& tempAllocator, App& app, IWindowHandler& windowHandler)
	{
		auto support = Bootstrap::QuerySwapChainSupport(tempAllocator, app);
		_extent = ChooseExtent(support.capabilities, windowHandler.GetResolution());

		const auto families = Bootstrap::GetQueueFamilies(tempAllocator, app);

		uint32_t queueFamilyIndices[] =
		{
			families.graphics,
			families.present
		};

		// Create a new swap chain based on the hardware requirements.
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = app.surface;
		createInfo.minImageCount = _images.GetLength();
		createInfo.imageFormat = _surfaceFormat.format;
		createInfo.imageColorSpace = _surfaceFormat.colorSpace;
		createInfo.imageExtent = _extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (families.graphics != families.present)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}

		createInfo.preTransform = support.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = _presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = _swapChain;

		VkSwapchainKHR newSwapChain;
		const auto result = vkCreateSwapchainKHR(app.logicalDevice, &createInfo, nullptr, &newSwapChain);
		assert(!result);

		support.Free(tempAllocator);
		Cleanup(app);

		_swapChain = newSwapChain;

		uint32_t length = _images.GetLength();
		jlb::Array<VkImage> vkImages{};
		vkImages.Allocate(tempAllocator, length);
		vkGetSwapchainImagesKHR(app.logicalDevice, _swapChain, &length, vkImages.GetData());

		const auto cmdBufferAllocInfo = CommandHandler::CreateBufferDefaultInfo(app);
		jlb::Array<VkCommandBuffer> cmdBuffers{};
		cmdBuffers.Allocate(tempAllocator, length);
		const auto cmdResult = vkAllocateCommandBuffers(app.logicalDevice, &cmdBufferAllocInfo, cmdBuffers.GetData());
		assert(!cmdResult);

		for (uint32_t i = 0; i < length; ++i)
		{
			auto& image = _images[i];
			image.colorImage = vkImages[i];

			auto viewCreateInfo = ImageHandler::CreateViewDefaultInfo();
			viewCreateInfo.image = image.colorImage;
			viewCreateInfo.format = _surfaceFormat.format;

			const auto viewResult = vkCreateImageView(app.logicalDevice, &viewCreateInfo, nullptr, &image.colorImageView);
			assert(!viewResult);

			image.cmdBuffer = cmdBuffers[i];
		}

		for (auto& frame : _frames)
		{
			auto semaphoreCreateInfo = SyncHandler::CreateSemaphoreDefaultInfo();
			auto semaphoreResult = vkCreateSemaphore(app.logicalDevice, &semaphoreCreateInfo, nullptr, &frame.imageAvailableSemaphore);
			assert(!semaphoreResult);
			semaphoreResult = vkCreateSemaphore(app.logicalDevice, &semaphoreCreateInfo, nullptr, &frame.renderFinishedSemaphore);
			assert(!semaphoreResult);

			auto fenceCreateInfo = SyncHandler::CreateFenceDefaultInfo();
			const auto fenceResult = vkCreateFence(app.logicalDevice, &fenceCreateInfo, nullptr, &frame.inFlightFence);
			assert(!fenceResult);
		}

		cmdBuffers.Free(tempAllocator);
		vkImages.Free(tempAllocator);
	}
}
