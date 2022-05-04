#include "pch.h"
#include "VkSwapChain.h"
#include "VkBootstrap.h"
#include "VkApp.h"
#include "JlbMath.h"
#include "VkImageHandler.h"
#include "VkSyncHandler.h"
#include "VkCommandHandler.h"
#include "VkFrameBufferHandler.h"
#include "VkRenderPassHandler.h"

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

	void SwapChain::WaitForImage(App& app)
	{
		auto& frame = _frames[_frameIndex];

		auto result = vkWaitForFences(app.logicalDevice, 1, &frame.inFlightFence, VK_TRUE, UINT64_MAX);
		assert(!result);
		result = vkAcquireNextImageKHR(app.logicalDevice,
			_swapChain, UINT64_MAX, frame.imageAvailableSemaphore, VK_NULL_HANDLE, &_imageIndex);
		assert(!result);

		auto& image = _images[_imageIndex];
		if(image.inFlightFence)
			vkWaitForFences(app.logicalDevice, 1, &image.inFlightFence, VK_TRUE, UINT64_MAX);
		image.inFlightFence = frame.inFlightFence;
	}

	VkCommandBuffer SwapChain::BeginFrame(App& app)
	{
		WaitForImage(app);

		auto& image = _images[_imageIndex];

		// Begin render command.
		auto cmd = CommandHandler::CreateBufferBeginDefaultInfo();
		vkResetCommandBuffer(image.cmdBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
		vkBeginCommandBuffer(image.cmdBuffer, &cmd);

		// Begin render pass.
		auto renderPassBeginInfo = RenderPassHandler::CreateBeginDefaultInfo();
		renderPassBeginInfo.renderPass = _renderPass;
		renderPassBeginInfo.framebuffer = image.frameBuffer;
		renderPassBeginInfo.renderArea.extent = _extent;
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;
		
		vkCmdBeginRenderPass(image.cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		return image.cmdBuffer;
	}

	VkResult SwapChain::EndFrame(jlb::LinearAllocator& tempAllocator, App& app, const jlb::ArrayView<VkSemaphore> waitSemaphores)
	{
		auto& frame = _frames[_frameIndex];
		auto& image = _images[_imageIndex];

		vkCmdEndRenderPass(image.cmdBuffer);

		auto result = vkEndCommandBuffer(image.cmdBuffer);
		assert(!result);

		jlb::Array<VkSemaphore> allWaitSemaphores{};
		allWaitSemaphores.Allocate(tempAllocator, waitSemaphores.length + 1);
		allWaitSemaphores.Copy(0, waitSemaphores.length, waitSemaphores.data);
		allWaitSemaphores[waitSemaphores.length] = frame.imageAvailableSemaphore;

		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		auto submitInfo = CommandHandler::CreateSubmitDefaultInfo(image.cmdBuffer);
		submitInfo.waitSemaphoreCount = allWaitSemaphores.GetLength();
		submitInfo.pWaitSemaphores = allWaitSemaphores.GetData();
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &frame.renderFinishedSemaphore;
;
		vkResetFences(app.logicalDevice, 1, &frame.inFlightFence);
		result = vkQueueSubmit(app.queues.graphics, 1, &submitInfo, frame.inFlightFence);
		assert(!result);

		allWaitSemaphores.Free(tempAllocator);

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &frame.renderFinishedSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &_swapChain;
		presentInfo.pImageIndices = &_imageIndex;

		result = vkQueuePresentKHR(app.queues.present, &presentInfo);
		_frameIndex = (_frameIndex + 1) % _frames.GetLength();
		return result;
	}

	void SwapChain::Cleanup(App& app)
	{
		if (!_swapChain)
			return;

		const auto result = vkDeviceWaitIdle(app.logicalDevice);
		assert(!result);

		for (auto& image : _images)
		{
			image.inFlightFence = VK_NULL_HANDLE;
			vkDestroyImageView(app.logicalDevice, image.colorImageView, nullptr);
			if (image.inFlightFence)
				vkWaitForFences(app.logicalDevice, 1, &image.inFlightFence, VK_TRUE, UINT64_MAX);
			vkFreeCommandBuffers(app.logicalDevice, app.commandPool, 1, &image.cmdBuffer);
			vkDestroyFramebuffer(app.logicalDevice, image.frameBuffer, nullptr);
		}

		for (auto& frame : _frames)
		{
			vkDestroySemaphore(app.logicalDevice, frame.imageAvailableSemaphore, nullptr);
			vkDestroySemaphore(app.logicalDevice, frame.renderFinishedSemaphore, nullptr);
			vkDestroyFence(app.logicalDevice, frame.inFlightFence, nullptr);
		}

		vkDestroyRenderPass(app.logicalDevice, _renderPass, nullptr);
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

		auto cmdBufferAllocInfo = CommandHandler::CreateBufferDefaultInfo(app);
		cmdBufferAllocInfo.commandBufferCount = length;
		jlb::Array<VkCommandBuffer> cmdBuffers{};
		cmdBuffers.Allocate(tempAllocator, length);
		const auto cmdResult = vkAllocateCommandBuffers(app.logicalDevice, &cmdBufferAllocInfo, cmdBuffers.GetData());
		assert(!cmdResult);

		// Create render pass.
		auto colorAttachmentReference = RenderPassHandler::CreateAttachmentReferenceDefaultInfo();
		auto subpassDescription = RenderPassHandler::CreateSubpassDescriptionDefaultInfo();
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentReference;

		auto subpassDependency = RenderPassHandler::CreateSubpassDependencyDefaultInfo();
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		auto attachmentDescription = RenderPassHandler::CreateAttachmentDescriptionDefaultInfo();
		attachmentDescription.format = _surfaceFormat.format;

		const auto renderPassCreateInfo = RenderPassHandler::CreateDefaultInfo(attachmentDescription, subpassDescription, subpassDependency);
		const auto renderPassresult = vkCreateRenderPass(app.logicalDevice, &renderPassCreateInfo, nullptr, &_renderPass);
		assert(!renderPassresult);

		// Create images.
		for (uint32_t i = 0; i < length; ++i)
		{
			auto& image = _images[i];
			image.colorImage = vkImages[i];

			auto viewCreateInfo = ImageHandler::CreateViewDefaultInfo(image.colorImage, _surfaceFormat.format);
			const auto viewResult = vkCreateImageView(app.logicalDevice, &viewCreateInfo, nullptr, &image.colorImageView);
			assert(!viewResult);

			image.cmdBuffer = cmdBuffers[i];

			auto frameBufferCreateInfo = FrameBufferHandler::CreateDefaultInfo();
			frameBufferCreateInfo.renderPass = _renderPass;
			frameBufferCreateInfo.attachmentCount = 1;
			frameBufferCreateInfo.pAttachments = &image.colorImageView;
			frameBufferCreateInfo.width = _extent.width;
			frameBufferCreateInfo.height = _extent.height;

			const auto frameBufferResult = vkCreateFramebuffer(app.logicalDevice, &frameBufferCreateInfo, nullptr, &image.frameBuffer);
			assert(!frameBufferResult);
		}

		// Create frames.
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

	VkRenderPass SwapChain::GetRenderPass() const
	{
		return _renderPass;
	}

	glm::ivec2 SwapChain::GetResolution() const
	{
		return {_extent.width, _extent.height};
	}

	uint8_t SwapChain::GetLength() const
	{
		return _images.GetLength();
	}

	uint8_t SwapChain::GetCurrentImageIndex() const
	{
		return _imageIndex;
	}
}
