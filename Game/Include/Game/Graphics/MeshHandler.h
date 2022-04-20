#pragma once
#include "VkRenderer/VkLinearAllocator.h"
#include "VkRenderer/VkCommandHandler.h"
#include "VkRenderer/VkSyncHandler.h"
#include "Mesh.h"
#include "VkRenderer/VkApp.h"

namespace game
{
	class MeshHandler final
	{
	public:
		template <typename Vertex, typename Index>
		[[nodiscard]] static Mesh Create(const EngineOutData& engineOutData, 
			jlb::ArrayView<Vertex> vertices, jlb::ArrayView<Index> indices);
		static void Destroy(const EngineOutData& engineOutData, Mesh& mesh);
	};

	template <typename Vertex, typename Index>
	Mesh MeshHandler::Create(const EngineOutData& engineOutData, 
		const jlb::ArrayView<Vertex> vertices, const jlb::ArrayView<Index> indices)
	{
		Mesh mesh{};

		auto& app = *engineOutData.app;
		auto& vkAllocator = *engineOutData.vkAllocator;

		// Vertex staging buffer.
		VkBufferCreateInfo vertBufferInfo{};
		vertBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertBufferInfo.size = sizeof(Vertex) * vertices.length;
		vertBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		vertBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		// Index staging buffer.
		VkBufferCreateInfo indBufferInfo{};
		indBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		indBufferInfo.size = sizeof(Index) * indices.length;
		indBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		indBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkBuffer vertStagingBuffer;
		VkBuffer indStagingBuffer;

		// Create buffers.
		auto result = vkCreateBuffer(app.logicalDevice, &vertBufferInfo, nullptr, &vertStagingBuffer);
		assert(!result);
		result = vkCreateBuffer(app.logicalDevice, &indBufferInfo, nullptr, &indStagingBuffer);
		assert(!result);

		// Get buffer requirements.
		VkMemoryRequirements vertStagingMemRequirements;
		vkGetBufferMemoryRequirements(app.logicalDevice, vertStagingBuffer, &vertStagingMemRequirements);
		VkMemoryRequirements indStagingMemRequirements;
		vkGetBufferMemoryRequirements(app.logicalDevice, indStagingBuffer, &indStagingMemRequirements);

		// Define and allocate memory.
		const uint32_t vertStagingBufferPoolId = vk::LinearAllocator::GetPoolId(app, vertStagingMemRequirements.memoryTypeBits, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		auto vertStagingBlock = vkAllocator.CreateBlock(vertBufferInfo.size, vertStagingBufferPoolId);
		const uint32_t indStagingBufferPoolId = vk::LinearAllocator::GetPoolId(app, indStagingMemRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		auto indStagingBlock = vkAllocator.CreateBlock(indBufferInfo.size, indStagingBufferPoolId);

		result = vkBindBufferMemory(app.logicalDevice, vertStagingBuffer, vertStagingBlock.memory, vertStagingBlock.offset);
		assert(!result);
		result = vkBindBufferMemory(app.logicalDevice, indStagingBuffer, indStagingBlock.memory, indStagingBlock.offset);
		assert(!result);

		// Move vertex/index data to a staging buffer.
		void* vertStagingData;
		vkMapMemory(app.logicalDevice, vertStagingBlock.memory, vertStagingBlock.offset, vertStagingBlock.size, 0, &vertStagingData);
		memcpy(vertStagingData, static_cast<const void*>(vertices.data), vertBufferInfo.size);
		vkUnmapMemory(app.logicalDevice, vertStagingBlock.memory);

		void* indStagingData;
		vkMapMemory(app.logicalDevice, indStagingBlock.memory, indStagingBlock.offset, indStagingBlock.size, 0, &indStagingData);
		memcpy(indStagingData, static_cast<const void*>(indices.data), indBufferInfo.size);
		vkUnmapMemory(app.logicalDevice, indStagingBlock.memory);

		// Create output buffers.
		vertBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		indBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

		result = vkCreateBuffer(app.logicalDevice, &vertBufferInfo, nullptr, &mesh.vertexBuffer);
		assert(!result);
		result = vkCreateBuffer(app.logicalDevice, &indBufferInfo, nullptr, &mesh.indexBuffer);
		assert(!result);

		VkMemoryRequirements vertMemRequirements;
		vkGetBufferMemoryRequirements(app.logicalDevice, mesh.vertexBuffer, &vertMemRequirements);
		VkMemoryRequirements indMemRequirements;
		vkGetBufferMemoryRequirements(app.logicalDevice, mesh.indexBuffer, &indMemRequirements);

		const uint32_t vertBufferPoolId = vk::LinearAllocator::GetPoolId(app, vertMemRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		mesh.vertexMemBlock = vkAllocator.CreateBlock(vertBufferInfo.size, vertBufferPoolId);
		const uint32_t indBufferPoolId = vk::LinearAllocator::GetPoolId(app, indMemRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		mesh.indexMemBlock = vkAllocator.CreateBlock(indBufferInfo.size, indBufferPoolId);

		result = vkBindBufferMemory(app.logicalDevice, mesh.vertexBuffer, mesh.vertexMemBlock.memory, mesh.vertexMemBlock.offset);
		assert(!result);
		result = vkBindBufferMemory(app.logicalDevice, mesh.indexBuffer, mesh.indexMemBlock.memory, mesh.indexMemBlock.offset);
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

		VkBufferCopy vertRegion{};
		vertRegion.srcOffset = 0;
		vertRegion.dstOffset = 0;
		vertRegion.size = mesh.vertexMemBlock.size;
		vkCmdCopyBuffer(cmdBuffer, vertStagingBuffer, mesh.vertexBuffer, 1, &vertRegion);

		VkBufferCopy indRegion{};
		indRegion.srcOffset = 0;
		indRegion.dstOffset = 0;
		indRegion.size = mesh.indexMemBlock.size;
		vkCmdCopyBuffer(cmdBuffer, indStagingBuffer, mesh.indexBuffer, 1, &indRegion);

		result = vkEndCommandBuffer(cmdBuffer);
		assert(!result);

		auto cmdSubmitInfo = vk::CommandHandler::CreateSubmitDefaultInfo(cmdBuffer);
		result = vkQueueSubmit(app.queues.graphics, 1, &cmdSubmitInfo, fence);
		assert(!result);

		result = vkWaitForFences(app.logicalDevice, 1, &fence, VK_TRUE, UINT64_MAX);
		assert(!result);
		vkDestroyFence(app.logicalDevice, fence, nullptr);

		vkDestroyBuffer(app.logicalDevice, indStagingBuffer, nullptr);
		vkDestroyBuffer(app.logicalDevice, vertStagingBuffer, nullptr);

		vkAllocator.FreeBlock(indStagingBlock);
		vkAllocator.FreeBlock(vertStagingBlock);

		mesh.indexCount = indices.length;
		return mesh;
	}
}
