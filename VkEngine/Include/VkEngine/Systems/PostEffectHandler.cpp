#include "VkEngine/pch.h"
#include "VkEngine/Graphics/PostEffectHandler.h"

#include "VkRenderer/VkApp.h"
#include "VkRenderer/VkImageUtils.h"
#include "VkRenderer/VkMemBlock.h"
#include "VkRenderer/VkStackAllocator.h"
#include "VkRenderer/VkSwapChain.h"

namespace vke
{
	void PostEffectHandler::Allocate(jlb::StackAllocator& allocator, vk::App& app, vk::SwapChain& swapChain, vk::StackAllocator& vkAllocator)
	{
		RecreateFrames(allocator, app, swapChain, vkAllocator);
	}

	void PostEffectHandler::Free(jlb::StackAllocator& allocator, vk::App& app, vk::SwapChain& swapChain, vk::StackAllocator& vkAllocator)
	{
		FreeFrames(allocator, app, swapChain, vkAllocator);
	}

	void PostEffectHandler::RecreateFrames(jlb::StackAllocator& allocator, vk::App& app, vk::SwapChain& swapChain, vk::StackAllocator& vkAllocator)
	{
		if(_frames)
		{
			FreeFrames(allocator, app, swapChain, vkAllocator);
		}

		const auto resolution = swapChain.GetResolution();
		const auto format = swapChain.GetFormat();
		const auto logicalDevice = app.logicalDevice;

		_frames.Allocate(allocator, swapChain.GetLength());
		for (auto& frame : _frames)
			for (auto& frameImage : frame.images)
			{
				auto& image = frameImage.image;
				auto& view = frameImage.view;
				auto& memBlock = frameImage.memBlock;

				// Create image.
				const auto imageInfo = vk::image::CreateDefaultInfo(resolution, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
				auto result = vkCreateImage(logicalDevice, &imageInfo, nullptr, &image);
				assert(!result);

				VkMemoryRequirements memRequirements;
				vkGetImageMemoryRequirements(logicalDevice, image, &memRequirements);
				const auto poolId = vkAllocator.GetPoolId(app, memRequirements.memoryTypeBits,
				                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
				memBlock = vkAllocator.AllocateBlock(app, memRequirements.size, memRequirements.alignment, poolId);

				result = vkBindImageMemory(logicalDevice, image, memBlock.memory, memBlock.offset);
				assert(!result);

				auto viewCreateInfo = vk::image::CreateViewDefaultInfo(image, format);
				const auto viewResult = vkCreateImageView(app.logicalDevice, &viewCreateInfo, nullptr, &view);
				assert(!viewResult);
			}
	}

	void PostEffectHandler::FreeFrames(jlb::StackAllocator& allocator, vk::App& app, vk::SwapChain& swapChain, vk::StackAllocator& vkAllocator)
	{
		const auto logicalDevice = app.logicalDevice;

		for (int32_t i = static_cast<int32_t>(_frames.GetLength()) - 1; i >= 0; --i)
		{
			const auto& frame = _frames[i];
			for (int32_t i = 1; i >= 0; --i)
			{
				auto& frameImage = frame.images[i];
				vkDestroyImageView(logicalDevice, frameImage.view, nullptr);
				vkDestroyImage(logicalDevice, frameImage.image, nullptr);
				vkAllocator.FreeBlock(frameImage.memBlock);
			}
		}
		_frames.Free(allocator);
	}
}
