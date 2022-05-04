#include "pch.h"
#include "Graphics/BaseRenderSystem.h"
#include "Graphics/MeshHandler.h"
#include "Graphics/PipelineHandler.h"
#include "FileLoader.h"
#include "Graphics/TextureHandler.h"
#include "VkRenderer/VkImageHandler.h"
#include "VkRenderer/VkSamplerHandler.h"
#include "Graphics/LayoutHandler.h"
#include "StackArray.h"

namespace game
{
	void BaseRenderSystem::Allocate(EngineOutData& engineOutData, CreateInfo& info)
	{
		LoadTextureAtlas(engineOutData, info.texturePath);
		LoadShader(engineOutData, info.shaderVertPath, info.shaderFragPath);
		_mesh = CreateMesh(engineOutData);
		CreateShaderAssets(engineOutData, info.instanceSize, info.instanceCount);
		CreateSwapChainAssets(engineOutData, info);
	}

	void BaseRenderSystem::Free(EngineOutData& engineOutData)
	{
		DestroySwapChainAssets(engineOutData);
		DestroyShaderAssets(engineOutData);
		MeshHandler::Destroy(engineOutData, _mesh);
		UnloadShader(engineOutData);
		UnloadTextureAtlas(engineOutData);
	}

	void BaseRenderSystem::Update(EngineOutData& engineOutData, void* instanceSrc, const size_t instanceSize, const size_t instanceCount)
	{
		auto& cmd = engineOutData.swapChainCommandBuffer;

		auto& logicalDevice = engineOutData.app->logicalDevice;
		auto& memBlock = _instanceMemBlocks[engineOutData.swapChainImageIndex];
		void* instanceData;
		const auto result = vkMapMemory(logicalDevice, memBlock.memory, memBlock.offset, memBlock.size, 0, &instanceData);
		assert(!result);
		memcpy(instanceSrc, instanceData, instanceSize * instanceCount);
		vkUnmapMemory(logicalDevice, memBlock.memory);

		VkDeviceSize offset = 0;
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout,
			0, 1, &_descriptorSets[engineOutData.swapChainImageIndex], 0, nullptr);
		vkCmdBindVertexBuffers(cmd, 0, 1, &_mesh.vertexBuffer, &offset);
		vkCmdBindIndexBuffer(cmd, _mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(cmd, _mesh.indexCount, instanceCount, 0, 0, 0);
	}

	void BaseRenderSystem::CreateSwapChainAssets(const EngineOutData& engineOutData, CreateInfo& info)
	{
		jlb::StackArray<PipelineHandler::Info::Module, 2> modules{};
		modules[0].module = _vertModule;
		modules[0].flags = VK_SHADER_STAGE_VERTEX_BIT;
		modules[1].module = _fragModule;
		modules[1].flags = VK_SHADER_STAGE_FRAGMENT_BIT;

		PipelineHandler::Info pipelineInfo{};
		pipelineInfo.vertInputAttribDescriptions = info.vertInputAttribDescriptions;
		pipelineInfo.vertInputBindingDescriptions = info.vertInputBindingDescriptions;
		pipelineInfo.resolution = engineOutData.resolution;
		pipelineInfo.modules = modules;
		pipelineInfo.renderPass = engineOutData.swapChainRenderPass;
		pipelineInfo.layouts = _descriptorLayout;

		PipelineHandler::Create(engineOutData, pipelineInfo, _pipelineLayout, _pipeline);
	}

	void BaseRenderSystem::DestroySwapChainAssets(const EngineOutData& engineOutData) const
	{
		auto& logicalDevice = engineOutData.app->logicalDevice;

		vkDestroyPipeline(logicalDevice, _pipeline, nullptr);
		vkDestroyPipelineLayout(logicalDevice, _pipelineLayout, nullptr);
	}

	void BaseRenderSystem::LoadShader(const EngineOutData& engineOutData, const jlb::StringView vertPath, const jlb::StringView fragPath)
	{
		auto& logicalDevice = engineOutData.app->logicalDevice;
		auto& tempAllocator = *engineOutData.tempAllocator;

		auto vert = jlb::FileLoader::Read(tempAllocator, vertPath);
		auto frag = jlb::FileLoader::Read(tempAllocator, fragPath);

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

	void BaseRenderSystem::UnloadShader(const EngineOutData& engineOutData) const
	{
		vkDestroyShaderModule(engineOutData.app->logicalDevice, _vertModule, nullptr);
		vkDestroyShaderModule(engineOutData.app->logicalDevice, _fragModule, nullptr);
	}

	void BaseRenderSystem::LoadTextureAtlas(const EngineOutData& engineOutData, const jlb::StringView path)
	{
		auto& app = *engineOutData.app;
		auto& logicalDevice = app.logicalDevice;

		_textureAtlas = TextureHandler::LoadTexture(engineOutData, path);
		const auto viewCreateInfo = vk::ImageHandler::CreateViewDefaultInfo(_textureAtlas.image, TextureHandler::GetTextureFormat());
		auto result = vkCreateImageView(logicalDevice, &viewCreateInfo, nullptr, &_atlasImageView);
		assert(!result);
		const auto samplerCreateInfo = vk::SamplerHandler::CreateDefaultInfo(app);
		result = vkCreateSampler(logicalDevice, &samplerCreateInfo, nullptr, &_atlasSampler);
		assert(!result);
	}

	void BaseRenderSystem::UnloadTextureAtlas(const EngineOutData& engineOutData)
	{
		auto& logicalDevice = engineOutData.app->logicalDevice;

		vkDestroySampler(logicalDevice, _atlasSampler, nullptr);
		vkDestroyImageView(logicalDevice, _atlasImageView, nullptr);
		TextureHandler::FreeTexture(engineOutData, _textureAtlas);
	}

	void BaseRenderSystem::CreateShaderAssets(const EngineOutData& engineOutData, const size_t instanceSize, const size_t instanceCount)
	{
		auto& app = *engineOutData.app;
		auto& allocator = *engineOutData.allocator;
		auto& tempAllocator = *engineOutData.tempAllocator;
		auto& vkAllocator = *engineOutData.vkAllocator;
		auto& logicalDevice = app.logicalDevice;

		const size_t swapChainImageCount = engineOutData.swapChainImageCount;

		// Create instance storage buffer.
		VkBufferCreateInfo vertBufferInfo{};
		vertBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertBufferInfo.size = instanceSize * instanceCount;
		vertBufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		vertBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		_instanceBuffers.Allocate(allocator, swapChainImageCount);
		_instanceMemBlocks.Allocate(allocator, swapChainImageCount);

		for (size_t i = 0; i < swapChainImageCount; ++i)
		{
			auto& buffer = _instanceBuffers[i];
			auto result = vkCreateBuffer(logicalDevice, &vertBufferInfo, nullptr, &buffer);
			assert(!result);

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

			const uint32_t poolId = vk::LinearAllocator::GetPoolId(app, memRequirements.memoryTypeBits,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			auto& memBlock = _instanceMemBlocks[i] = vkAllocator.CreateBlock(app, vertBufferInfo.size, memRequirements.alignment, poolId);

			result = vkBindBufferMemory(logicalDevice, buffer, memBlock.memory, memBlock.offset);
			assert(!result);
		}

		// Create descriptor layout.
		jlb::StackArray<LayoutHandler::Info::Binding, 2> bindings{};
		bindings[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		bindings[0].size = instanceSize * instanceCount;
		bindings[0].flag = VK_SHADER_STAGE_VERTEX_BIT;
		bindings[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		bindings[1].flag = VK_SHADER_STAGE_FRAGMENT_BIT;

		LayoutHandler::Info descriptorLayoutInfo{};
		descriptorLayoutInfo.bindings = bindings;
		_descriptorLayout = LayoutHandler::Create(engineOutData, descriptorLayoutInfo);

		// Create descriptor pool.
		VkDescriptorPoolSize poolSize{};
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

#ifdef _DEBUG
		// Really dumb, but updating the descriptor sets just crashes the debug build, 
		// and Vulkan doesn't give a reason as to why.
		return;
#endif

		// Bind descriptor sets to the instance data.
		for (size_t i = 0; i < swapChainImageCount; ++i)
		{
			jlb::StackArray<VkWriteDescriptorSet, 2> writes{};

			// Bind instance buffer.
			VkDescriptorBufferInfo instanceInfo{};
			instanceInfo.buffer = _instanceBuffers[i];
			instanceInfo.offset = 0;
			instanceInfo.range = instanceSize * instanceCount;

			auto& instanceWrite = writes[0];
			instanceWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			instanceWrite.dstBinding = 0;
			instanceWrite.dstSet = _descriptorSets[i];
			instanceWrite.descriptorCount = 1;
			instanceWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			instanceWrite.pBufferInfo = &instanceInfo;
			instanceWrite.dstArrayElement = 0;

			// Bind texture atlas.
			VkDescriptorImageInfo  atlasInfo{};
			atlasInfo.imageLayout = TextureHandler::GetImageLayout();
			atlasInfo.imageView = _atlasImageView;
			atlasInfo.sampler = _atlasSampler;

			auto& textureAtlasWrite = writes[1];
			textureAtlasWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			textureAtlasWrite.dstBinding = 1;
			textureAtlasWrite.dstSet = _descriptorSets[i];
			textureAtlasWrite.descriptorCount = 1;
			textureAtlasWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			textureAtlasWrite.pImageInfo = &atlasInfo;
			textureAtlasWrite.dstArrayElement = 0;

			vkUpdateDescriptorSets(logicalDevice, writes.GetLength(), writes.GetData(), 0, nullptr);
		}
	}

	void BaseRenderSystem::DestroyShaderAssets(const EngineOutData& engineOutData)
	{
		auto& app = *engineOutData.app;
		auto& logicalDevice = app.logicalDevice;
		auto& allocator = *engineOutData.allocator;
		auto& vkAllocator = *engineOutData.vkAllocator;

		vkDestroyDescriptorPool(logicalDevice, _descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(logicalDevice, _descriptorLayout, nullptr);
		_descriptorSets.Free(*engineOutData.allocator);

		for (int32_t i = engineOutData.swapChainImageCount - 1; i >= 0; --i)
		{
			vkDestroyBuffer(logicalDevice, _instanceBuffers[i], nullptr);
			vkAllocator.FreeBlock(_instanceMemBlocks[i]);
		}

		_instanceMemBlocks.Free(allocator);
		_instanceBuffers.Free(allocator);
	}
}
