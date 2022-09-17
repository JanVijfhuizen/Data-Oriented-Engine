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
#include "Heap.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <fstream>

namespace vke::texture
{
	Texture _Load(const EngineData& info, const jlb::ArrayView<stbi_uc> pixels, const glm::ivec2 resolution)
	{
		auto& app = *info.app;
		auto& logicalDevice = app.logicalDevice;
		auto& vkAllocator = *info.vkAllocator;

		const size_t imageSize = static_cast<size_t>(resolution.x) * static_cast<size_t>(resolution.y) * 4;

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
		memcpy(data, pixels.data, imageSize);
		vkUnmapMemory(logicalDevice, stagingMemBlock.memory);

		// Create image.
		auto imageInfo = vk::image::CreateDefaultInfo(resolution, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

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
		vk::image::CopyBufferToImage(stagingBuffer, image, cmdBuffer, resolution);
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
		texture.resolution = resolution;
		return texture;
	}

	void GenerateAtlas(const EngineData& info, 
		const jlb::StringView texturePath, const jlb::StringView subtexturePath,
		const jlb::ArrayView<TextureAtlasPartition> partitions,
		const size_t nodeResolution, const size_t atlasWidth)
	{
		// Check if atlas width is power of 2.
		assert((atlasWidth & (atlasWidth - 1)) == 0);
		assert(atlasWidth != 0);

		struct FilledNode final
		{
			glm::ivec2 coordinates{};
			TextureAtlasPartition partition{};
			size_t index;
		};

		jlb::Vector<FilledNode> filledNodes{};
		filledNodes.Allocate(*info.tempAllocator, partitions.length);

		{
			struct Node final
			{
				glm::ivec2 coordinates{};
				size_t width{};
			};

			struct Content final
			{
				TextureAtlasPartition partition{};
				size_t index{};
			};

			jlb::Heap<Node> open{};
			open.Allocate(*info.tempAllocator, partitions.length + 1);

			// Add full grid as starting point.
			{
				Node start{};
				start.width = atlasWidth;
				open.Insert(start, start.width);
			}

			jlb::Heap<Content> heap{};
			heap.Allocate(*info.tempAllocator, partitions.length);

			// Sort partitions from largest to smallest.
			{
				size_t index = 0;
				for (auto& partition : partitions)
				{
					Content content{};
					content.partition = partition;
					content.index = index++;
					heap.Insert(content, SIZE_MAX - partition.width);
				}
			}

			jlb::Vector<Node> ignored{};
			ignored.Allocate(*info.tempAllocator, open.GetLength());

			size_t layersUsed = 0;

			// While there are still sub textures to partition.
			while (heap.GetCount())
			{
				const auto content = heap.Pop();
				const auto& partition = content.partition;

				// While there is still space left.
				bool found = false;
				while (open.GetCount())
				{
					auto node = open.Pop();
					if (node.width >= partition.width)
					{
						// Found a place to put it in.
						FilledNode filledNode{};
						filledNode.coordinates = node.coordinates;
						filledNode.partition = partition;
						filledNode.index = content.index;
						filledNodes.Add(filledNode);

						// Add the new smaller node if it still exists.
						node.coordinates.x += static_cast<int>(partition.width);
						node.width -= partition.width;
						if(node.width > 0)
							open.Insert(node, node.width);
						found = true;
						break;
					}

					ignored.Add(node);
				}

				// If no more space is present in the current layer.
				if(!found)
				{
					Node newLayerStart{};
					newLayerStart.coordinates.x = static_cast<int>(partition.width);
					newLayerStart.coordinates.y = static_cast<int>(++layersUsed);
					newLayerStart.width = atlasWidth - partition.width;
					open.Insert(newLayerStart, newLayerStart.width);

					FilledNode filledNode{};
					filledNode.coordinates.y = static_cast<int>(layersUsed);
					filledNode.partition = partition;
					filledNode.index = content.index;
					filledNodes.Add(filledNode);
				}

				// Add the ignored back into the heap.
				for (auto& node : ignored)
					open.Insert(node, node.width);
				ignored.SetCount(0);
			}

			ignored.Free(*info.tempAllocator);
			heap.Free(*info.tempAllocator);
			open.Free(*info.tempAllocator);

			assert(atlasWidth >= layersUsed);
		}

		jlb::Array<stbi_uc> atlasPixels{};
		atlasPixels.Allocate(*info.tempAllocator, static_cast<size_t>(powf(static_cast<float>(atlasWidth) * nodeResolution, 2)) * 4);

		const size_t yStepSize = atlasWidth * nodeResolution * 4;

		for (const auto& node : filledNodes)
		{
			// Load pixels.
			int texWidth, texHeight, texChannels;
			stbi_uc* pixels = stbi_load(node.partition.path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			assert(pixels);
			assert(texChannels == 4);

			const auto& coordinates = node.coordinates;
			const size_t width = node.partition.width * nodeResolution * 4;
			const size_t startIndex = (static_cast<size_t>(coordinates.x) * 4 + coordinates.y * yStepSize) * nodeResolution;

			for (size_t y = 0; y < nodeResolution; ++y)
			{
				const size_t yStartIndex = startIndex + y * yStepSize;
				jlb::Copy(atlasPixels.GetView(), yStartIndex, yStartIndex + width, &pixels[y * width]);
			}

			// Free pixels.
			stbi_image_free(pixels);
		}

		stbi_write_png(texturePath, atlasWidth * nodeResolution,
			atlasWidth * nodeResolution, 4, atlasPixels.GetData(), 0);

		atlasPixels.Free(*info.tempAllocator);

		jlb::Heap<SubTexture> subTextures{};
		subTextures.Allocate(*info.tempAllocator, filledNodes.GetLength());

		for (auto& filledNode : filledNodes)
		{
			SubTexture subTexture{};
			subTexture.lTop = filledNode.coordinates;
			subTexture.rBot = filledNode.coordinates + glm::ivec2(filledNode.partition.width, 1);
			subTexture.lTop /= atlasWidth;
			subTexture.rBot /= atlasWidth;
			subTextures.Insert(subTexture, filledNode.index);
		}

		std::ofstream outfile;
		outfile.open(subtexturePath);

		while(subTextures.GetCount())
		{
			const auto subTexture = subTextures.Pop();
			outfile << subTexture.lTop.x << std::endl;
			outfile << subTexture.lTop.y << std::endl;
			outfile << subTexture.rBot.x << std::endl;
			outfile << subTexture.rBot.y << std::endl;
		}

		outfile.close();

		subTextures.Free(*info.tempAllocator);
		filledNodes.Free(*info.tempAllocator);
	}

	void LoadAtlasSubTextures(const jlb::StringView coordsPath, const jlb::ArrayView<SubTexture> outSubTextures)
	{
		std::ifstream infile;
		infile.open(coordsPath);

		for (size_t i = 0; i < outSubTextures.length; ++i)
		{
			auto& coord = outSubTextures[i];
			infile >> coord.lTop.x;
			infile >> coord.lTop.y;
			infile >> coord.rBot.x;
			infile >> coord.rBot.y;
		}

		infile.close();
	}

	void Subdivide(const SubTexture subTexture, const size_t amount, const jlb::ArrayView<SubTexture> outSubTextures)
	{
		const float partitionSize = (subTexture.rBot.x - subTexture.lTop.x) / static_cast<float>(amount);
		for (size_t i = 0; i < amount; ++i)
		{
			auto& newSubTexture = outSubTextures[i];
			newSubTexture = subTexture;
			newSubTexture.rBot.x = newSubTexture.lTop.x;
			newSubTexture.lTop.x += partitionSize * i;
			newSubTexture.rBot.x += partitionSize * (i + 1);
		}
	}

	float GetChunkSize(const SubTexture& subTexture, const size_t amount)
	{
		return (subTexture.rBot.x - subTexture.lTop.x) / amount;
	}

	glm::vec2 GetCenter(const SubTexture& subTexture)
	{
		return subTexture.lTop + (subTexture.rBot - subTexture.lTop) * .5f;
	}

	SubTexture MirrorHorizontally(const SubTexture& subTexture)
	{
		SubTexture ret = subTexture;
		ret.lTop.x = subTexture.rBot.x;
		ret.rBot.x = subTexture.lTop.x;
		return ret;
	}

	SubTexture ToSubTexture(const glm::vec2& position, const glm::vec2& scale)
	{
		SubTexture ret{};
		const glm::vec2 halfScale = scale * .5f;
		ret.lTop = position + glm::vec2(-halfScale.x, halfScale.y);
		ret.rBot = position + glm::vec2(halfScale.x, -halfScale.y);
		return ret;
	}

	SubTexture GetSubTexture(const SubTexture& subTexture, const size_t amount, const size_t index)
	{
		SubTexture ret = subTexture;
		const float chunkSize = GetChunkSize(subTexture, amount);
		ret.lTop.x += chunkSize * static_cast<float>(index);
		ret.rBot.x = ret.lTop.x + chunkSize;
		return ret;
	}

	Texture Load(const EngineData& info, const jlb::StringView path)
	{
		// Load pixels.
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		const VkDeviceSize imageSize = texWidth * texHeight * 4;
		assert(pixels);

		jlb::ArrayView<stbi_uc> view{};
		view.data = pixels;
		view.length = imageSize;

		const auto ret = _Load(info, view, {texWidth, texHeight});
		stbi_image_free(pixels);
		return ret;
	}

	void Free(const EngineData& info, const Texture& texture)
	{
		vkDestroyImage(info.app->logicalDevice, texture.image, nullptr);
		info.vkAllocator->FreeBlock(texture.memBlock);
	}
}
