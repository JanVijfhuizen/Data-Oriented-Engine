#pragma once
#include "VkRenderer/VkLinearAllocator.h"

namespace game
{
	class MeshHandler final
	{
	public:
		template <typename Vertex, typename Index>
		static void Create(const EngineOutData& engineOutData, 
			jlb::ArrayView<Vertex> vertices, jlb::ArrayView<Index> indices, 
			VkBuffer& outVertBuffer, VkBuffer& outIndBuffer);
	};

	template <typename Vertex, typename Index>
	void MeshHandler::Create(const EngineOutData& engineOutData, 
		const jlb::ArrayView<Vertex> vertices, const jlb::ArrayView<Index> indices, 
		VkBuffer& outVertBuffer, VkBuffer& outIndBuffer)
	{
		auto& app = *engineOutData.app;
		auto& vkAllocator = *engineOutData.vkAllocator;

		// Vertex buffer.
		VkBufferCreateInfo vertBufferInfo{};
		vertBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertBufferInfo.size = sizeof(Vertex) * vertices.length;
		vertBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		vertBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		// Index buffer.
		VkBufferCreateInfo indBufferInfo{};
		indBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		indBufferInfo.size = sizeof(Index) * indices.length;
		indBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
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
		const auto vertStagingBlock = vkAllocator.CreateBlock(vertBufferInfo.size, vertStagingBufferPoolId);
		const uint32_t indStagingBufferPoolId = vk::LinearAllocator::GetPoolId(app, indStagingMemRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		const auto indStagingBlock = vkAllocator.CreateBlock(indBufferInfo.size, indStagingBufferPoolId);

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
		memcpy(vertStagingData, static_cast<const void*>(vertices.data), indBufferInfo.size);
		vkUnmapMemory(app.logicalDevice, indStagingBlock.memory);
	}
}
