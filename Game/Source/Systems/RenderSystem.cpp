#include "pch.h"
#include "Systems/RenderSystem.h"
#include "FileLoader.h"
#include "StringView.h"
#include "VkRenderer/VkApp.h"
#include "Graphics/Vertex.h"
#include "Graphics/PipelineHandler.h"
#include "Graphics/MeshHandler.h"
#include "Components/Transform.h"

namespace game
{
	void RenderSystem::Allocate(const EngineOutData& engineOutData)
	{
		TaskSystem::Allocate(*engineOutData.allocator);
		LoadShader(engineOutData);
		CreateMesh(engineOutData);
		CreateInstanceArray(engineOutData);
		CreateSwapChainAssets(engineOutData);
	}

	void RenderSystem::Free(const EngineOutData& engineOutData)
	{
		DestroySwapChainAssets(engineOutData);
		DestroyInstanceArray(engineOutData);
		MeshHandler::Destroy(engineOutData, _mesh);
		UnloadShader(engineOutData);
		TaskSystem::Free(*engineOutData.allocator);
	}

	void RenderSystem::Update(const EngineOutData& engineOutData)
	{
		const VkDeviceSize instanceOffset = _instanceMemBlock.offset + _instanceMemBlock.size / engineOutData.swapChainImageCount * engineOutData.swapChainImageIndex;
		void* instanceData;
		const auto result = vkMapMemory(engineOutData.app->logicalDevice, _instanceMemBlock.memory, instanceOffset, _instanceMemBlock.size, 0, &instanceData);
		assert(!result);
		memcpy(instanceData, static_cast<const void*>(GetData()), GetLength() * sizeof(RenderTask));
		vkUnmapMemory(engineOutData.app->logicalDevice, _instanceMemBlock.memory);

		jlb::StackArray<VkBuffer, 2> vertexBuffers{};
		vertexBuffers[0] = _mesh.vertexBuffer;
		vertexBuffers[1] = _instanceBuffer;
		jlb::StackArray<VkDeviceSize, 2> offsets{};

		vkCmdBindPipeline(engineOutData.swapChainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
		vkCmdBindVertexBuffers(engineOutData.swapChainCommandBuffer, 0, 2, vertexBuffers.GetData(), offsets.GetData());
		vkCmdBindIndexBuffer(engineOutData.swapChainCommandBuffer, _mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(engineOutData.swapChainCommandBuffer, _mesh.indexCount, GetCount(), 0, 0, 0);
		SetCount(0);
	}

	RenderTask RenderSystem::CreateDefaultTask(Renderer& renderer, Transform& transform)
	{
		RenderTask task{};
		auto& vertexInstance = task.vertexInstance;
		vertexInstance.position = transform.position;
		vertexInstance.rotation = transform.rotation;
		vertexInstance.scale = transform.scale;
		return task;
	}

	void RenderSystem::LoadShader(const EngineOutData& engineOutData)
	{
		auto vert = jlb::FileLoader::Read(*engineOutData.tempAllocator, "Shaders/vert.spv");
		auto frag = jlb::FileLoader::Read(*engineOutData.tempAllocator, "Shaders/frag.spv");

		VkShaderModuleCreateInfo vertCreateInfo{};
		vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vertCreateInfo.codeSize = vert.GetLength();
		vertCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vert.GetData());

		VkShaderModuleCreateInfo fragCreateInfo{};
		fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		fragCreateInfo.codeSize = frag.GetLength();
		fragCreateInfo.pCode = reinterpret_cast<const uint32_t*>(frag.GetData());

		auto result = vkCreateShaderModule(engineOutData.app->logicalDevice, &vertCreateInfo, nullptr, &_vertModule);
		assert(!result);
		result = vkCreateShaderModule(engineOutData.app->logicalDevice, &fragCreateInfo, nullptr, &_fragModule);
		assert(!result);

		frag.Free(*engineOutData.tempAllocator);
		vert.Free(*engineOutData.tempAllocator);
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

	void RenderSystem::CreateInstanceArray(const EngineOutData& engineOutData)
	{
		auto& app = *engineOutData.app;
		auto& vkAllocator = *engineOutData.vkAllocator;

		VkBufferCreateInfo vertBufferInfo{};
		vertBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertBufferInfo.size = sizeof(RenderTask) * GetLength() * engineOutData.swapChainImageCount;
		vertBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		vertBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		auto result = vkCreateBuffer(app.logicalDevice, &vertBufferInfo, nullptr, &_instanceBuffer);
		assert(!result);

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(app.logicalDevice, _instanceBuffer, &memRequirements);

		const uint32_t poolId = vk::LinearAllocator::GetPoolId(app, memRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		_instanceMemBlock = vkAllocator.CreateBlock(vertBufferInfo.size, poolId);

		result = vkBindBufferMemory(app.logicalDevice, _instanceBuffer, _instanceMemBlock.memory, _instanceMemBlock.offset);
		assert(!result);
	}

	void RenderSystem::DestroyInstanceArray(const EngineOutData& engineOutData)
	{
		auto& app = *engineOutData.app;
		auto& vkAllocator = *engineOutData.vkAllocator;

		vkAllocator.FreeBlock(_instanceMemBlock);
		vkDestroyBuffer(app.logicalDevice, _instanceBuffer, nullptr);
	}

	void RenderSystem::CreateSwapChainAssets(const EngineOutData& engineOutData)
	{
		jlb::StackArray<PipelineHandler::Info::Module, 2> modules{};
		modules[0].module = _vertModule;
		modules[0].flags = VK_SHADER_STAGE_VERTEX_BIT;
		modules[1].module = _fragModule;
		modules[1].flags = VK_SHADER_STAGE_FRAGMENT_BIT;

		PipelineHandler::Info pipelineInfo{};
		pipelineInfo.vertInputAttribDescriptions = Vertex::GetAttributeDescriptions();
		pipelineInfo.vertInputBindingDescriptions = Vertex::GetBindingDescriptions();
		pipelineInfo.resolution = engineOutData.resolution;
		pipelineInfo.modules = modules;
		pipelineInfo.renderPass = engineOutData.swapChainRenderPass;

		PipelineHandler::CreatePipeline(engineOutData, pipelineInfo, _pipelineLayout, _pipeline);
	}

	void RenderSystem::DestroySwapChainAssets(const EngineOutData& engineOutData) const
	{
		vkDestroyPipeline(engineOutData.app->logicalDevice, _pipeline, nullptr);
		vkDestroyPipelineLayout(engineOutData.app->logicalDevice, _pipelineLayout, nullptr);
	}
}
