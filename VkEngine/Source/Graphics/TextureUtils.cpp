#include "VkEngine/pch.h"
#include "VkEngine/Graphics/TextureUtils.h"
#include "VkRenderer/VkCommandBufferUtils.h"
#include "VkRenderer/VkImageUtils.h"
#include "VkRenderer/VkStackAllocator.h"
#include "VkEngine/Graphics/Texture.h"
#include "VkRenderer/VkApp.h"
#include "VkRenderer/VkSyncUtils.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "StringView.h"

namespace vke::texture
{
	Texture Load(const EngineData& info, const jlb::StringView path)
	{
		auto& app = *info.app;
		auto& logicalDevice = app.logicalDevice;
		auto& vkAllocator = *info.vkAllocator;

		// Load pixels.
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		const VkDeviceSize imageSize = texWidth * texHeight * 4;
		assert(pixels);

		// Create staging buffer.
		VkBuffer stagingBuffer;
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = imageSize;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		auto result = vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &stagingBuffer);
		assert(!result);

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(logicalDevice, stagingBuffer, &memRequirements);

		const auto stagingPoolId = vkAllocator.GetPoolId(app, memRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		const auto stagingMemBlock = vkAllocator.AllocateBlock(app, memRequirements.size, memRequirements.alignment, stagingPoolId);

		result = vkBindBufferMemory(logicalDevice, stagingBuffer, stagingMemBlock.memory, stagingMemBlock.offset);
		assert(!result);

		// Copy pixels to staging buffer.
		void* data;
		vkMapMemory(logicalDevice, stagingMemBlock.memory, stagingMemBlock.offset, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(logicalDevice, stagingMemBlock.memory);

		// Free pixels.
		stbi_image_free(pixels);

		// Create image.
		auto imageInfo = vk::image::CreateDefaultInfo({ texWidth, texHeight }, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

		VkImage image;
		result = vkCreateImage(logicalDevice, &imageInfo, nullptr, &image);
		assert(!result);

		vkGetImageMemoryRequirements(logicalDevice, image, &memRequirements);
		const auto poolId = vkAllocator.GetPoolId(app, memRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		const auto memBlock = vkAllocator.AllocateBlock(app, memRequirements.size, memRequirements.alignment, poolId);

		result = vkBindImageMemory(logicalDevice, image, memBlock.memory, memBlock.offset);
		assert(!result);

		// Start transfer.
		VkCommandBuffer cmdBuffer;
		auto cmdBufferAllocInfo = vk::cmdBuffer::CreateDefaultInfo(app);
		result = vkAllocateCommandBuffers(app.logicalDevice, &cmdBufferAllocInfo, &cmdBuffer);
		assert(!result);

		VkFence fence;
		auto fenceInfo = vk::sync::CreateFenceDefaultInfo();
		result = vkCreateFence(app.logicalDevice, &fenceInfo, nullptr, &fence);
		assert(!result);
		result = vkResetFences(app.logicalDevice, 1, &fence);
		assert(!result);

		// Begin recording.
		auto cmdBeginInfo = vk::cmdBuffer::CreateBeginDefaultInfo();
		vkBeginCommandBuffer(cmdBuffer, &cmdBeginInfo);

		vk::image::TransitionLayout(image, cmdBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
		vk::image::CopyBufferToImage(stagingBuffer, image, cmdBuffer, { texWidth, texHeight });
		vk::image::TransitionLayout(image, cmdBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

		// End recording.
		result = vkEndCommandBuffer(cmdBuffer);
		assert(!result);

		// Submit.
		auto cmdSubmitInfo = vk::cmdBuffer::CreateSubmitDefaultInfo(cmdBuffer);
		result = vkQueueSubmit(app.queues.graphics, 1, &cmdSubmitInfo, fence);
		assert(!result);

		result = vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, UINT64_MAX);
		assert(!result);
		vkDestroyFence(logicalDevice, fence, nullptr);

		// Destroy staging buffer.
		vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
		vkAllocator.FreeBlock(stagingMemBlock);

		Texture texture{};
		texture.image = image;
		texture.memBlock = memBlock;
		texture.resolution = { texWidth, texHeight };
		return texture;
	}

	void Free(const EngineData& info, const Texture& texture)
	{
		vkDestroyImage(info.app->logicalDevice, texture.image, nullptr);
		info.vkAllocator->FreeBlock(texture.memBlock);
	}
}
