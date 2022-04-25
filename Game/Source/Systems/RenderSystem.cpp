#include "pch.h"
#include "Systems/RenderSystem.h"
#include "FileLoader.h"
#include "StringView.h"
#include "VkRenderer/VkApp.h"
#include "Graphics/Vertex.h"
#include "Graphics/PipelineHandler.h"
#include "Graphics/MeshHandler.h"
#include "Components/Transform.h"
#include "Graphics/LayoutHandler.h"

namespace game
{
	void RenderSystem::Allocate(const EngineOutData& engineOutData)
	{
		TaskSystem::Allocate(*engineOutData.allocator);
		LoadShader(engineOutData);
		CreateMesh(engineOutData);
		CreateShaderAssets(engineOutData);
		CreateSwapChainAssets(engineOutData);
	}

	void RenderSystem::Free(const EngineOutData& engineOutData)
	{
		DestroySwapChainAssets(engineOutData);
		DestroyShaderAssets(engineOutData);
		MeshHandler::Destroy(engineOutData, _mesh);
		UnloadShader(engineOutData);
		TaskSystem::Free(*engineOutData.allocator);
	}

	void RenderSystem::Update(const EngineOutData& engineOutData)
	{
		auto& cmd = engineOutData.swapChainCommandBuffer;

		auto& logicalDevice = engineOutData.app->logicalDevice;
		const VkDeviceSize instanceOffset = _instanceMemBlock.offset + _instanceMemBlock.size / engineOutData.swapChainImageCount * engineOutData.swapChainImageIndex;
		void* instanceData;
		const auto result = vkMapMemory(logicalDevice, _instanceMemBlock.memory, instanceOffset, _instanceMemBlock.size, 0, &instanceData);
		assert(!result);
		memcpy(instanceData, static_cast<const void*>(GetData()), sizeof(RenderTask) * GetLength());
		vkUnmapMemory(logicalDevice, _instanceMemBlock.memory);

		VkDeviceSize offset = 0;

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
		vkCmdBindVertexBuffers(cmd, 0, 1, &_mesh.vertexBuffer, &offset);
		vkCmdBindIndexBuffer(cmd, _mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(cmd, _mesh.indexCount, GetCount(), 0, 0, 0);
		SetCount(0);
	}

	RenderTask RenderSystem::CreateDefaultTask(Renderer& renderer, Transform& transform)
	{
		RenderTask task{};
		auto& taskTransform = task.transform;
		taskTransform.position = transform.position;
		taskTransform.rotation = transform.rotation;
		taskTransform.scale = transform.scale;
		return task;
	}

	void RenderSystem::LoadShader(const EngineOutData& engineOutData)
	{
		auto& logicalDevice = engineOutData.app->logicalDevice;
		auto& tempAllocator = *engineOutData.tempAllocator;

		auto vert = jlb::FileLoader::Read(tempAllocator, "Shaders/vert.spv");
		auto frag = jlb::FileLoader::Read(tempAllocator, "Shaders/frag.spv");

		VkShaderModuleCreateInfo vertCreateInfo{};
		vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vertCreateInfo.codeSize = vert.GetLength();
		vertCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vert.GetData());

		VkShaderModuleCreateInfo fragCreateInfo{};
		fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		fragCreateInfo.codeSize = frag.GetLength();
		fragCreateInfo.pCode = reinterpret_cast<const uint32_t*>(frag.GetData());

		auto result = vkCreateShaderModule(logicalDevice, &vertCreateInfo, nullptr, &_vertModule);
		assert(!result);
		result = vkCreateShaderModule(logicalDevice, &fragCreateInfo, nullptr, &_fragModule);
		assert(!result);

		frag.Free(tempAllocator);
		vert.Free(tempAllocator);
	}

	void RenderSystem::UnloadShader(const EngineOutData& engineOutData) const
	{
		vkDestroyShaderModule(engineOutData.app->logicalDevice, _vertModule, nullptr);
		vkDestroyShaderModule(engineOutData.app->logicalDevice, _fragModule, nullptr);
	}

	void RenderSystem::CreateMesh(const EngineOutData& engineOutData)
	{
		jlb::StackArray<Vertex, 4> vertices{};
		vertices[0].position = { -1, -1 };
		vertices[1].position = { -1, 1 };
		vertices[2].position = { 1, 1 };
		vertices[3].position = { 1, -1 };
		vertices[0].textureCoordinates = { 0, 0 };
		vertices[1].textureCoordinates = { 0, 1 };
		vertices[2].textureCoordinates = { 1, 1 };
		vertices[3].textureCoordinates = { 1, 0 };
		jlb::StackArray<Vertex::Index, 6> indices{};
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 0;
		indices[4] = 2;
		indices[5] = 3;

		_mesh = MeshHandler::Create<Vertex, Vertex::Index>(engineOutData, vertices, indices);
	}

	void RenderSystem::CreateShaderAssets(const EngineOutData& engineOutData)
	{
		auto& app = *engineOutData.app;
		auto& vkAllocator = *engineOutData.vkAllocator;
		auto& logicalDevice = app.logicalDevice;

		// Create instance storage buffer.
		VkBufferCreateInfo vertBufferInfo{};
		vertBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertBufferInfo.size = sizeof(RenderTask) * GetLength() * engineOutData.swapChainImageCount;
		vertBufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		vertBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		auto result = vkCreateBuffer(logicalDevice, &vertBufferInfo, nullptr, &_instanceBuffer);
		assert(!result);

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(logicalDevice, _instanceBuffer, &memRequirements);

		const uint32_t poolId = vk::LinearAllocator::GetPoolId(app, memRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		_instanceMemBlock = vkAllocator.CreateBlock(vertBufferInfo.size, poolId);

		result = vkBindBufferMemory(logicalDevice, _instanceBuffer, _instanceMemBlock.memory, _instanceMemBlock.offset);
		assert(!result);

		// Create descriptor layout.
		jlb::StackArray<LayoutHandler::Info::Binding, 1> bindings{};
		bindings[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		bindings[0].size = sizeof(RenderTask) * GetLength();
		bindings[0].flag = VK_SHADER_STAGE_VERTEX_BIT;
		LayoutHandler::Info descriptorLayoutInfo{};
		descriptorLayoutInfo.bindings = bindings;

		_descriptorLayout = LayoutHandler::Create(engineOutData, descriptorLayoutInfo);

		// Create descriptor pool.
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSize.descriptorCount = 1;
		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = 1;

		result = vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &_descriptorPool);
		assert(!result);

		// Create descriptor set.
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &_descriptorLayout;

		result = vkAllocateDescriptorSets(logicalDevice, &allocInfo, &_descriptorSet);
		assert(!result);
	}

	void RenderSystem::DestroyShaderAssets(const EngineOutData& engineOutData)
	{
		auto& app = *engineOutData.app;
		auto& logicalDevice = app.logicalDevice;
		auto& vkAllocator = *engineOutData.vkAllocator;

		vkDestroyDescriptorPool(logicalDevice, _descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(logicalDevice, _descriptorLayout, nullptr);

		vkAllocator.FreeBlock(_instanceMemBlock);
		vkDestroyBuffer(logicalDevice, _instanceBuffer, nullptr);
	}

	void RenderSystem::CreateSwapChainAssets(const EngineOutData& engineOutData)
	{
		jlb::StackArray<PipelineHandler::Info::Module, 2> modules{};
		modules[0].module = _vertModule;
		modules[0].flags = VK_SHADER_STAGE_VERTEX_BIT;
		modules[1].module = _fragModule;
		modules[1].flags = VK_SHADER_STAGE_FRAGMENT_BIT;

		auto vertAttributes = Vertex::GetAttributeDescriptions();
		auto vertBindings = Vertex::GetBindingDescriptions();

		PipelineHandler::Info pipelineInfo{};
		pipelineInfo.vertInputAttribDescriptions = vertAttributes;
		pipelineInfo.vertInputBindingDescriptions = vertBindings;
		pipelineInfo.resolution = engineOutData.resolution;
		pipelineInfo.modules = modules;
		pipelineInfo.renderPass = engineOutData.swapChainRenderPass;
		//pipelineInfo.layouts = _descriptorLayout;

		PipelineHandler::Create(engineOutData, pipelineInfo, _pipelineLayout, _pipeline);
	}

	void RenderSystem::DestroySwapChainAssets(const EngineOutData& engineOutData) const
	{
		auto& logicalDevice = engineOutData.app->logicalDevice;

		vkDestroyPipeline(logicalDevice, _pipeline, nullptr);
		vkDestroyPipelineLayout(logicalDevice, _pipelineLayout, nullptr);
	}
}
