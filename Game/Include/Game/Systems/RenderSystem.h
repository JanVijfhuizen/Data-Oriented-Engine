#pragma once
#include "TaskSystem.h"
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"
#include "VkRenderer/VkMemBlock.h"
#include "StringView.h"
#include "VkRenderer/VkApp.h"
#include "Graphics/PipelineHandler.h"
#include "Graphics/Vertex.h"
#include "Graphics/MeshHandler.h"
#include "FileLoader.h"
#include "Graphics/TextureHandler.h"
#include "VkRenderer/VkImageHandler.h"
#include "VkRenderer/VkSamplerHandler.h"
#include "Graphics/LayoutHandler.h"

namespace game
{
	template <typename Task>
	class RenderSystem : public TaskSystem<Task>
	{
	public:
		struct CreateInfo final
		{
			jlb::StringView atlasTexturePath;
			jlb::StringView vertPath;
			jlb::StringView fragPath;
		};

		struct UpdateInfo final
		{
			glm::vec2 cameraPosition{};
			float pixelSize = 0.008f;
		};

		UpdateInfo updateInfo{};

		void CreateSwapChainAssets(const EngineOutData& engineOutData);
		void DestroySwapChainAssets(const EngineOutData& engineOutData) const;

		[[nodiscard]] const Texture& GetTexture() const;

	protected:
		[[nodiscard]] virtual CreateInfo GetCreateInfo() = 0;

		void Update(const EngineOutData& outData, SystemChain& chain) override;
		void Allocate(const EngineOutData& outData, SystemChain& chain) override;
		void Free(const EngineOutData& outData, SystemChain& chain) override;

	private:
		struct PushConstant final
		{
			glm::vec2 resolution;
			UpdateInfo updateInfo;
		};

		VkShaderModule _vertModule;
		VkShaderModule _fragModule;
		Texture _textureAtlas;
		VkImageView _atlasImageView;
		VkSampler _atlasSampler;
		Mesh _mesh;
		VkDescriptorPool _descriptorPool;
		VkDescriptorSetLayout _descriptorLayout;
		jlb::Array<VkDescriptorSet> _descriptorSets{};
		jlb::Array<VkBuffer> _instanceBuffers{};
		jlb::Array<vk::MemBlock> _instanceMemBlocks{};

		VkPipelineLayout _pipelineLayout;
		VkPipeline _pipeline;

		void LoadShader(const EngineOutData& engineOutData, const CreateInfo& createInfo);
		void UnloadShader(const EngineOutData& engineOutData) const;
		void LoadTextureAtlas(const EngineOutData& engineOutData, const CreateInfo& createInfo);
		void UnloadTextureAtlas(const EngineOutData& engineOutData);

		void CreateMesh(const EngineOutData& engineOutData);
		void CreateShaderAssets(const EngineOutData& engineOutData);
		void DestroyShaderAssets(const EngineOutData& engineOutData);
	};

	template <typename Task>
	void RenderSystem<Task>::CreateSwapChainAssets(const EngineOutData& engineOutData)
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
		pipelineInfo.layouts = _descriptorLayout;
		pipelineInfo.pushConstantSize = sizeof(PushConstant);
		pipelineInfo.usePushConstant = true;

		PipelineHandler::Create(engineOutData, pipelineInfo, _pipelineLayout, _pipeline);
	}

	template <typename Task>
	void RenderSystem<Task>::DestroySwapChainAssets(const EngineOutData& engineOutData) const
	{
		auto& logicalDevice = engineOutData.app->logicalDevice;

		vkDestroyPipeline(logicalDevice, _pipeline, nullptr);
		vkDestroyPipelineLayout(logicalDevice, _pipelineLayout, nullptr);
	}

	template <typename Task>
	const Texture& RenderSystem<Task>::GetTexture() const
	{
		return _textureAtlas;
	}

	template <typename Task>
	void RenderSystem<Task>::Update(const EngineOutData& outData, SystemChain& chain)
	{
		auto& cmd = outData.swapChainCommandBuffer;

		auto& logicalDevice = outData.app->logicalDevice;
		auto& memBlock = _instanceMemBlocks[outData.swapChainImageIndex];
		void* instanceData;
		const auto result = vkMapMemory(logicalDevice, memBlock.memory, memBlock.offset, memBlock.size, 0, &instanceData);
		assert(!result);
		memcpy(instanceData, static_cast<const void*>(TaskSystem<Task>::GetData()), sizeof(Task) * TaskSystem<Task>::GetLength());
		vkUnmapMemory(logicalDevice, memBlock.memory);

		VkDeviceSize offset = 0;
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout,
			0, 1, &_descriptorSets[outData.swapChainImageIndex], 0, nullptr);
		vkCmdBindVertexBuffers(cmd, 0, 1, &_mesh.vertexBuffer, &offset);
		vkCmdBindIndexBuffer(cmd, _mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT16);

		PushConstant pushConstant{};
		pushConstant.resolution = outData.resolution;
		pushConstant.updateInfo = updateInfo;

		vkCmdPushConstants(cmd, _pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstant), &pushConstant);
		vkCmdDrawIndexed(cmd, _mesh.indexCount, TaskSystem<Task>::GetCount(), 0, 0, 0);

		TaskSystem<Task>::SetCount(0);
	}

	template <typename Task>
	void RenderSystem<Task>::Allocate(const EngineOutData& outData, SystemChain& chain)
	{
		const CreateInfo createInfo = GetCreateInfo();

		TaskSystem<Task>::Allocate(outData, chain);
		LoadTextureAtlas(outData, createInfo);
		LoadShader(outData, createInfo);
		CreateMesh(outData);
		CreateShaderAssets(outData);
		CreateSwapChainAssets(outData);
	}

	template <typename Task>
	void RenderSystem<Task>::Free(const EngineOutData& outData, SystemChain& chain)
	{
		DestroySwapChainAssets(outData);
		DestroyShaderAssets(outData);
		MeshHandler::Destroy(outData, _mesh);
		UnloadShader(outData);
		UnloadTextureAtlas(outData);
		TaskSystem<Task>::Free(outData, chain);
	}

	template <typename Task>
	void RenderSystem<Task>::LoadShader(const EngineOutData& engineOutData, const CreateInfo& createInfo)
	{
		auto& logicalDevice = engineOutData.app->logicalDevice;
		auto& tempAllocator = *engineOutData.tempAllocator;

		auto vert = jlb::FileLoader::Read(tempAllocator, createInfo.vertPath);
		auto frag = jlb::FileLoader::Read(tempAllocator, createInfo.fragPath);

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

	template <typename Task>
	void RenderSystem<Task>::UnloadShader(const EngineOutData& engineOutData) const
	{
		vkDestroyShaderModule(engineOutData.app->logicalDevice, _vertModule, nullptr);
		vkDestroyShaderModule(engineOutData.app->logicalDevice, _fragModule, nullptr);
	}

	template <typename Task>
	void RenderSystem<Task>::LoadTextureAtlas(const EngineOutData& engineOutData, const CreateInfo& createInfo)
	{
		auto& app = *engineOutData.app;
		auto& logicalDevice = app.logicalDevice;

		_textureAtlas = TextureHandler::LoadTexture(engineOutData, createInfo.atlasTexturePath);
		const auto viewCreateInfo = vk::ImageHandler::CreateViewDefaultInfo(_textureAtlas.image, TextureHandler::GetTextureFormat());
		auto result = vkCreateImageView(logicalDevice, &viewCreateInfo, nullptr, &_atlasImageView);
		assert(!result);
		const auto samplerCreateInfo = vk::SamplerHandler::CreateDefaultInfo(app);
		result = vkCreateSampler(logicalDevice, &samplerCreateInfo, nullptr, &_atlasSampler);
		assert(!result);
	}

	template <typename Task>
	void RenderSystem<Task>::UnloadTextureAtlas(const EngineOutData& engineOutData)
	{
		auto& logicalDevice = engineOutData.app->logicalDevice;

		vkDestroySampler(logicalDevice, _atlasSampler, nullptr);
		vkDestroyImageView(logicalDevice, _atlasImageView, nullptr);
		TextureHandler::FreeTexture(engineOutData, _textureAtlas);
	}

	template <typename Task>
	void RenderSystem<Task>::CreateMesh(const EngineOutData& engineOutData)
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

	template <typename Task>
	void RenderSystem<Task>::CreateShaderAssets(const EngineOutData& engineOutData)
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
		vertBufferInfo.size = sizeof(Task) * TaskSystem<Task>::GetLength();
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

			const uint32_t poolId = vkAllocator.GetPoolId(app, memRequirements.memoryTypeBits,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			auto& memBlock = _instanceMemBlocks[i] = vkAllocator.AllocateBlock(app, memRequirements.size, memRequirements.alignment, poolId);

			result = vkBindBufferMemory(logicalDevice, buffer, memBlock.memory, memBlock.offset);
			assert(!result);
		}

		// Create descriptor layout.
		jlb::StackArray<LayoutHandler::Info::Binding, 2> bindings{};
		bindings[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		bindings[0].size = sizeof(Task) * TaskSystem<Task>::GetLength();
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

		// Bind descriptor sets to the instance data.
		for (size_t i = 0; i < swapChainImageCount; ++i)
		{
			jlb::StackArray<VkWriteDescriptorSet, 2> writes{};

			// Bind instance buffer.
			VkDescriptorBufferInfo instanceInfo{};
			instanceInfo.buffer = _instanceBuffers[i];
			instanceInfo.offset = 0;
			instanceInfo.range = sizeof(Task) * TaskSystem<Task>::GetLength();

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

	template <typename Task>
	void RenderSystem<Task>::DestroyShaderAssets(const EngineOutData& engineOutData)
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
