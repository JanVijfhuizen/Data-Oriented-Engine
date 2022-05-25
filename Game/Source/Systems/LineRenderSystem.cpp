#include "pch.h"
#include "Systems/LineRenderSystem.h"
#include "Handlers/ShaderHandler.h"
#include "StringView.h"
#include "StackArray.h"
#include "Graphics/PipelineHandler.h"
#include "VkRenderer/VkApp.h"
#include "Graphics/MeshHandler.h"
#include "Graphics/LayoutHandler.h"
#include "Graphics/InstanceUtils.h"

namespace game
{
	void LineRenderSystem::Allocate(const EngineOutData& outData, SystemChain& chain)
	{
		TaskSystem<LineRenderTask>::Allocate(outData, chain);
		_shader = ShaderHandler::Create(outData, "Shaders/debug-vert.spv","Shaders/debug-frag.spv");

		// Create mesh.
		jlb::StackArray<int, 2> vertices{};
		vertices[0] = 0;
		vertices[1] = 1;
		_vertexBuffer = MeshHandler::Create<int>(outData, vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		CreateShaderAssets(outData);
	}

	void LineRenderSystem::Free(const EngineOutData& outData, SystemChain& chain)
	{
		DestroyShaderAssets(outData);
		FreeBuffer(outData, _vertexBuffer);
		ShaderHandler::Destroy(outData, _shader);
		TaskSystem<LineRenderTask>::Free(outData, chain);
	}

	void LineRenderSystem::Update(const EngineOutData& outData, SystemChain& chain)
	{
		auto& cameras = *chain.Get<CameraArchetype>();
		for (const auto& camera : cameras)
		{
			auto& cmd = outData.swapChainCommandBuffer;

			auto& logicalDevice = outData.app->logicalDevice;
			auto& memBlock = _instanceBuffers[outData.swapChainImageIndex].memBlock;
			void* instanceData;
			const auto result = vkMapMemory(logicalDevice, memBlock.memory, memBlock.offset, memBlock.size, 0, &instanceData);
			assert(!result);
			memcpy(instanceData, static_cast<const void*>(GetData()), sizeof(LineRenderTask) * GetLength());
			vkUnmapMemory(logicalDevice, memBlock.memory);

			VkDeviceSize offset = 0;
			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout,
				0, 1, &_descriptorSets[outData.swapChainImageIndex], 0, nullptr);
			vkCmdBindVertexBuffers(cmd, 0, 1, &_vertexBuffer.buffer, &offset);

			PushConstants pushConstant{};
			pushConstant.resolution = outData.resolution;
			pushConstant.camera = camera;

			vkCmdPushConstants(cmd, _pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &pushConstant);
			vkCmdDraw(cmd, 2, GetCount(), 0, 0);
		}
		SetCount(0);
	}

	void LineRenderSystem::CreateShaderAssets(const EngineOutData& outData)
	{
		auto& app = *outData.app;
		auto& allocator = *outData.allocator;
		auto& tempAllocator = *outData.tempAllocator;
		auto& logicalDevice = app.logicalDevice;
		const size_t swapChainImageCount = outData.swapChainImageCount;

		_instanceBuffers = CreateInstanceStorageBuffers<LineRenderTask>(outData, GetLength());

		// Create descriptor layout.
		LayoutHandler::Info::Binding binding{};
		binding.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		binding.size = sizeof(LineRenderTask) * GetLength();
		binding.flag = VK_SHADER_STAGE_VERTEX_BIT;

		LayoutHandler::Info descriptorLayoutInfo{};
		descriptorLayoutInfo.bindings = binding;
		_descriptorLayout = LayoutHandler::Create(outData, descriptorLayoutInfo);

		// Create descriptor pool.
		VkDescriptorPoolSize poolSize;
		poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSize.descriptorCount = swapChainImageCount;
		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = swapChainImageCount;

		auto result = vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &_descriptorPool);
		assert(!result);

		jlb::Array<VkDescriptorSetLayout> layouts{};
		layouts.Allocate(tempAllocator, swapChainImageCount, _descriptorLayout);

		// Create descriptor set.
		_descriptorSets.Allocate(allocator, swapChainImageCount);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _descriptorPool;
		allocInfo.descriptorSetCount = swapChainImageCount;
		allocInfo.pSetLayouts = layouts.GetData();

		result = vkAllocateDescriptorSets(logicalDevice, &allocInfo, _descriptorSets.GetData());
		assert(!result);

		layouts.Free(tempAllocator);

		// Bind descriptor sets to the instance data.
		for (size_t i = 0; i < swapChainImageCount; ++i)
		{
			VkWriteDescriptorSet write{};

			// Bind instance buffer.
			VkDescriptorBufferInfo instanceInfo{};
			instanceInfo.buffer = _instanceBuffers[i].buffer;
			instanceInfo.offset = 0;
			instanceInfo.range = sizeof(LineRenderTask) * GetLength();

			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstBinding = 0;
			write.dstSet = _descriptorSets[i];
			write.descriptorCount = 1;
			write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			write.pBufferInfo = &instanceInfo;
			write.dstArrayElement = 0;

			vkUpdateDescriptorSets(logicalDevice, 1, &write, 0, nullptr);
		}
	}

	void LineRenderSystem::DestroyShaderAssets(const EngineOutData& outData)
	{
		auto& app = *outData.app;
		auto& logicalDevice = app.logicalDevice;
		auto& allocator = *outData.allocator;

		vkDestroyDescriptorPool(logicalDevice, _descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(logicalDevice, _descriptorLayout, nullptr);
		_descriptorSets.Free(*outData.allocator);

		for (int32_t i = outData.swapChainImageCount - 1; i >= 0; --i)
			FreeBuffer(outData, _instanceBuffers[i]);

		_instanceBuffers.Free(allocator);
	}

	void LineRenderSystem::CreateSwapChainAssets(const EngineOutData& outData, SystemChain& chain)
	{
		TaskSystem<LineRenderTask>::CreateSwapChainAssets(outData, chain);
		jlb::StackArray<PipelineHandler::Info::Module, 2> modules{};
		modules[0].module = _shader.vert;
		modules[0].flags = VK_SHADER_STAGE_VERTEX_BIT;
		modules[1].module = _shader.frag;
		modules[1].flags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(int);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VkVertexInputAttributeDescription attributeDescription{};
		attributeDescription.binding = 0;
		attributeDescription.location = 0;
		attributeDescription.format = VK_FORMAT_R32_SINT;
		attributeDescription.offset = 0;

		PipelineHandler::Info pipelineInfo{};
		pipelineInfo.vertInputAttribDescriptions = attributeDescription;
		pipelineInfo.vertInputBindingDescriptions = bindingDescription;
		pipelineInfo.resolution = outData.resolution;
		pipelineInfo.modules = modules;
		pipelineInfo.renderPass = outData.swapChainRenderPass;
		pipelineInfo.layouts = _descriptorLayout;
		pipelineInfo.pushConstantSize = sizeof(PushConstants);
		pipelineInfo.usePushConstant = true;
		pipelineInfo.topologyType = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

		PipelineHandler::Create(outData, pipelineInfo, _pipelineLayout, _pipeline);
	}

	void LineRenderSystem::DestroySwapChainAssets(const EngineOutData& outData, SystemChain& chain)
	{
		auto& logicalDevice = outData.app->logicalDevice;

		vkDestroyPipeline(logicalDevice, _pipeline, nullptr);
		vkDestroyPipelineLayout(logicalDevice, _pipelineLayout, nullptr);

		TaskSystem<LineRenderTask>::DestroySwapChainAssets(outData, chain);
	}
}
