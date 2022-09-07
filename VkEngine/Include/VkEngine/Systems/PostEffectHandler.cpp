#include "VkEngine/pch.h"
#include "VkEngine/Graphics/PostEffectHandler.h"

#include "VkEngine/Graphics/LayoutUtils.h"
#include "VkRenderer/VkApp.h"
#include "VkRenderer/VkImageUtils.h"
#include "VkRenderer/VkMemBlock.h"
#include "VkRenderer/VkRenderPassUtils.h"
#include "VkRenderer/VkStackAllocator.h"
#include "VkRenderer/VkSwapChain.h"

namespace vke
{
	void PostEffect::Allocate(const vk::App& app, PostEffectHandler& handler, const Shader& shader)
	{
		_shader = shader;
	}

	void PostEffect::Free(const vk::App& app, PostEffectHandler& handler)
	{
	}

	void PostEffect::Recreate(const vk::App& app, PostEffectHandler& handler)
	{
	}

	void PostEffectHandler::Allocate(const EngineData& info, vk::SwapChain& swapChain, vk::StackAllocator& vkAllocator)
	{
		// Create descriptor layout.
		layout::Info::Binding binding{};
		binding.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding.flag = VK_SHADER_STAGE_FRAGMENT_BIT;

		layout::Info descriptorLayoutInfo{};
		descriptorLayoutInfo.bindings = binding;
		_descriptorLayout = layout::Create(info, descriptorLayoutInfo);

		const uint32_t swapChainImageCount = swapChain.GetLength();

		// Create descriptor pool.
		VkDescriptorPoolSize poolSize;
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = swapChainImageCount; // todo base on game post effects.
		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = swapChainImageCount;

		const auto result = vkCreateDescriptorPool(info.app->logicalDevice, &poolInfo, nullptr, &_descriptorPool);
		assert(!result);

		RecreateFrames(*info.allocator, *info.app, swapChain, vkAllocator);
	}

	void PostEffectHandler::Free(jlb::StackAllocator& allocator, const vk::App& app, vk::SwapChain& swapChain, const vk::StackAllocator& vkAllocator)
	{
		FreeFrames(allocator, app, swapChain, vkAllocator);
		vkDestroyDescriptorPool(app.logicalDevice, _descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(app.logicalDevice, _descriptorLayout, nullptr);
	}

	void PostEffectHandler::RecreateFrames(jlb::StackAllocator& allocator, const vk::App& app, vk::SwapChain& swapChain, vk::StackAllocator& vkAllocator)
	{
		if(_frames)
			FreeFrames(allocator, app, swapChain, vkAllocator);

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
				auto imageInfo = vk::image::CreateDefaultInfo(resolution, VK_IMAGE_USAGE_SAMPLED_BIT);
				imageInfo.format = format;
				auto result = vkCreateImage(logicalDevice, &imageInfo, nullptr, &image);
				assert(!result);

				VkMemoryRequirements memRequirements;
				vkGetImageMemoryRequirements(logicalDevice, image, &memRequirements);
				const auto poolId = vkAllocator.GetPoolId(app, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
				memBlock = vkAllocator.AllocateBlock(app, memRequirements.size, memRequirements.alignment, poolId);

				result = vkBindImageMemory(logicalDevice, image, memBlock.memory, memBlock.offset);
				assert(!result);

				auto viewCreateInfo = vk::image::CreateViewDefaultInfo(image, format);
				const auto viewResult = vkCreateImageView(app.logicalDevice, &viewCreateInfo, nullptr, &view);
				assert(!viewResult);
			}

		// Create render pass.
		auto imageAttachmentDescription = vk::renderPass::CreateAttachmentDescriptionDefaultInfo();
		imageAttachmentDescription.format = format;
		imageAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		auto colorAttachmentReference = vk::renderPass::CreateAttachmentReferenceDefaultInfo();
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		auto subpassDescription = vk::renderPass::CreateSubpassDescriptionDefaultInfo();
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentReference;
		auto subpassDependency = vk::renderPass::CreateSubpassDependencyDefaultInfo();
		auto renderPassCreateInfo = vk::renderPass::CreateDefaultInfo(
			imageAttachmentDescription, subpassDescription, subpassDependency);
		const auto result = vkCreateRenderPass(app.logicalDevice, &renderPassCreateInfo, nullptr, &_renderPass);
		assert(!result);
	}

	void PostEffectHandler::FreeFrames(jlb::StackAllocator& allocator, const vk::App& app, vk::SwapChain& swapChain, const vk::StackAllocator& vkAllocator)
	{
		const auto logicalDevice = app.logicalDevice;

		vkDestroyRenderPass(logicalDevice, _renderPass, nullptr);
		for (int32_t i = static_cast<int32_t>(_frames.GetLength()) - 1; i >= 0; --i)
		{
			const auto& frame = _frames[i];
			for (int32_t j = 1; j >= 0; --j)
			{
				auto& frameImage = frame.images[j];
				vkDestroyImageView(logicalDevice, frameImage.view, nullptr);
				vkDestroyImage(logicalDevice, frameImage.image, nullptr);
				vkAllocator.FreeBlock(frameImage.memBlock);
			}
		}
		_frames.Free(allocator);
	}
}
