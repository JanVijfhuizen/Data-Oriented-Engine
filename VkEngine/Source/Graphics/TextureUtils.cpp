#include "VkEngine/pch.h"
#include "VkEngine/Graphics/TextureUtils.h"
#include "VkRenderer/VkCommandBufferUtils.h"
#include "VkRenderer/VkImageUtils.h"
#include "VkRenderer/VkStackAllocator.h"
#include "VkEngine/Graphics/Texture.h"
#include "VkRenderer/VkApp.h"
#include "VkRenderer/VkSyncUtils.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include "StringView.h"
#include "Heap.h"

namespace vke::texture
{
	// Testing.
		/*
		// Load pixels.
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(partitions[0].path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		const VkDeviceSize imageSize = texWidth * texHeight * 4;
		assert(pixels);

		stbi_write_jpg("atlas_test.jpg", texWidth, texHeight, 4, pixels, 100);

		stbi_image_free(pixels);
		*/

	Texture LoadAsAtlas(const EngineData& info, const jlb::ArrayView<TextureAtlasPartition> partitions,
		const jlb::ArrayView<SubTexture> outSubTextures, const glm::ivec2 nodeResolution, const size_t atlasWidth)
	{
		struct FilledNode final
		{
			glm::ivec2 coordinates{};
			TextureAtlasPartition partition{};
		};

		jlb::Vector<FilledNode> filledNodes{};
		filledNodes.Allocate(*info.tempAllocator, partitions.length);

		{
			struct Node final
			{
				glm::ivec2 coordinates{};
				size_t width{};
			};

			jlb::Heap<Node> open{};
			open.Allocate(*info.tempAllocator, partitions.length + 1);

			// Add full grid as starting point.
			{
				Node start{};
				start.width = atlasWidth;
				open.Insert(start, start.width);
			}

			jlb::Heap<TextureAtlasPartition> heap{};
			heap.Allocate(*info.tempAllocator, partitions.length);

			// Sort partitions from largest to smallest.
			for (auto& partition : partitions)
				heap.Insert(partition, partition.width);

			jlb::Vector<Node> ignored{};
			ignored.Allocate(*info.tempAllocator, open.GetLength());

			while (heap.GetCount())
			{
				const auto partition = heap.Pop();

				while (open.GetCount())
				{
					auto node = open.Pop();
					if (node.width >= partition.width)
					{
						// Found a place to put it in.
						FilledNode filledNode{};
						filledNode.coordinates = node.coordinates;
						filledNode.partition = partition;
						filledNodes.Add(filledNode);

						node.width -= partition.width;
						if(node.width > 0)
							open.Insert(node, node.width);
						break;
					}

					ignored.Add(node);
				}
			}

			heap.Free(*info.tempAllocator);
			open.Free(*info.tempAllocator);
		}

		filledNodes.Free(*info.tempAllocator);

		return {};
	}

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
