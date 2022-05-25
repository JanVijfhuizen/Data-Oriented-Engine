#pragma once
#include "VkRenderer/VkStackAllocator.h"
#include "VkRenderer/VkCommandHandler.h"
#include "VkRenderer/VkSyncHandler.h"
#include "Mesh.h"
#include "VkRenderer/VkApp.h"
#include "Buffer.h"

namespace game
{
	class MeshHandler final
	{
	public:
		/// <summary>
		/// Creates a buffer in device local memory.
		/// </summary>
		/// <param name="usageFlags">Automatically includes DST usage.</param>
		template <typename Type>
		[[nodiscard]] static Buffer Create(const EngineOutData& outData, jlb::ArrayView<Type> vertices, VkBufferUsageFlags usageFlags);

		/// <summary>
		/// Creates a vertex and index buffer in device local memory.
		/// </summary>
		template <typename Vertex, typename Index>
		[[nodiscard]] static Mesh CreateIndexed(const EngineOutData& outData, 
			jlb::ArrayView<Vertex> vertices, jlb::ArrayView<Index> indices);
		static void Destroy(const EngineOutData& outData, Mesh& mesh);
	};

	template <typename Type>
	Buffer MeshHandler::Create(const EngineOutData& outData, const jlb::ArrayView<Type> vertices, const VkBufferUsageFlags usageFlags)
	{
		auto& app = *outData.app;
		auto& vkAllocator = *outData.vkAllocator;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = sizeof(Type) * vertices.length;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkBuffer stagingBuffer;
		auto result = vkCreateBuffer(app.logicalDevice, &bufferInfo, nullptr, &stagingBuffer);
		assert(!result);

		// Get buffer requirements.
		VkMemoryRequirements stagingMemRequirements;
		vkGetBufferMemoryRequirements(app.logicalDevice, stagingBuffer, &stagingMemRequirements);

		// Define and allocate memory.
		const uint32_t vertStagingBufferPoolId = vkAllocator.GetPoolId(app, stagingMemRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		const auto stagingBlock = vkAllocator.AllocateBlock(app, bufferInfo.size, stagingMemRequirements.alignment, vertStagingBufferPoolId);

		result = vkBindBufferMemory(app.logicalDevice, stagingBuffer, stagingBlock.memory, stagingBlock.offset);
		assert(!result);

		// Move vertex/index data to a staging buffer.
		void* stagingData;
		vkMapMemory(app.logicalDevice, stagingBlock.memory, stagingBlock.offset, stagingBlock.size, 0, &stagingData);
		memcpy(stagingData, static_cast<const void*>(vertices.data), bufferInfo.size);
		vkUnmapMemory(app.logicalDevice, stagingBlock.memory);

		bufferInfo.usage = usageFlags | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		VkBuffer buffer;
		result = vkCreateBuffer(app.logicalDevice, &bufferInfo, nullptr, &buffer);
		assert(!result);

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(app.logicalDevice, buffer, &memRequirements);

		const uint32_t vertBufferPoolId = vkAllocator.GetPoolId(app, memRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		const auto memBlock = vkAllocator.AllocateBlock(app, bufferInfo.size, memRequirements.alignment, vertBufferPoolId);

		result = vkBindBufferMemory(app.logicalDevice, buffer, memBlock.memory, memBlock.offset);
		assert(!result);

		VkCommandBuffer cmdBuffer;
		auto cmdBufferAllocInfo = vk::CommandHandler::CreateBufferDefaultInfo(app);
		result = vkAllocateCommandBuffers(app.logicalDevice, &cmdBufferAllocInfo, &cmdBuffer);
		assert(!result);

		VkFence fence;
		auto fenceInfo = vk::SyncHandler::CreateFenceDefaultInfo();
		result = vkCreateFence(app.logicalDevice, &fenceInfo, nullptr, &fence);
		assert(!result);
		result = vkResetFences(app.logicalDevice, 1, &fence);
		assert(!result);

		auto cmdBeginInfo = vk::CommandHandler::CreateBufferBeginDefaultInfo();
		vkBeginCommandBuffer(cmdBuffer, &cmdBeginInfo);

		VkBufferCopy region{};
		region.srcOffset = 0;
		region.dstOffset = 0;
		region.size = memBlock.size;
		vkCmdCopyBuffer(cmdBuffer, stagingBuffer, buffer, 1, &region);

		result = vkEndCommandBuffer(cmdBuffer);
		assert(!result);

		auto cmdSubmitInfo = vk::CommandHandler::CreateSubmitDefaultInfo(cmdBuffer);
		result = vkQueueSubmit(app.queues.graphics, 1, &cmdSubmitInfo, fence);
		assert(!result);

		result = vkWaitForFences(app.logicalDevice, 1, &fence, VK_TRUE, UINT64_MAX);
		assert(!result);

		vkDestroyFence(app.logicalDevice, fence, nullptr);
		vkDestroyBuffer(app.logicalDevice, stagingBuffer, nullptr);
		vkAllocator.FreeBlock(stagingBlock);

		Buffer meshBuffer{};
		meshBuffer.buffer = buffer;
		meshBuffer.memBlock = memBlock;
		return meshBuffer;
	}

	template <typename Vertex, typename Index>
	Mesh MeshHandler::CreateIndexed(const EngineOutData& outData, 
		const jlb::ArrayView<Vertex> vertices, const jlb::ArrayView<Index> indices)
	{
		Mesh mesh{};
		mesh.vertexBuffer = Create(outData, vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		mesh.indexBuffer = Create(outData, indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
		mesh.indexCount = indices.length;
		return mesh;
	}
}
