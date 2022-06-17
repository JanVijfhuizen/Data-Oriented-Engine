#pragma once
#include "TaskSystem.h"
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"
#include "VkRenderer/VkMemBlock.h"
#include "StringView.h"
#include "VkRenderer/VkApp.h"
#include "Graphics/PipelineUtils.h"
#include "Graphics/Vertex.h"
#include "Graphics/MeshUtils.h"
#include "FileLoader.h"
#include "Graphics/TextureUtils.h"
#include "VkRenderer/VkImageUtils.h"
#include "VkRenderer/VkSamplerUtils.h"
#include "Graphics/LayoutUtils.h"
#include "Graphics/Shader.h"
#include "Handlers/ShaderHandler.h"
#include "Graphics/InstancingUtils.h"
#include "Archetypes/CameraArchetype.h"

namespace game
{
	/// <summary>
	/// Handles the rendering of sprites.<br>
	/// Any rendering system that inherits from this must adhere to the shader formatting used in this system.
	/// </summary>
	template <typename Task>
	class RenderSystem : public TaskSystem<Task>
	{
	public:
		// Used to define the renderer.
		struct CreateInfo final
		{
			jlb::StringView atlasTexturePath;
			jlb::StringView vertPath;
			jlb::StringView fragPath;
		};

		// Get the texture atlas used for this renderer.
		[[nodiscard]] const Texture& GetTexture() const;

	protected:
		[[nodiscard]] virtual CreateInfo GetCreateInfo() = 0;

		void Allocate(const EngineData& EngineData, SystemChain& chain) override;
		void Free(const EngineData& EngineData, SystemChain& chain) override;
		void Update(const EngineData& EngineData, SystemChain& chain) override;

		void CreateSwapChainAssets(const EngineData& EngineData, SystemChain& chain) override;
		void DestroySwapChainAssets(const EngineData& EngineData, SystemChain& chain) override;

	private:
		struct PushConstants final
		{
			glm::vec2 resolution;
			Camera camera;
		};

		Shader _shader;
		Texture _textureAtlas;
		VkImageView _atlasImageView;
		VkSampler _atlasSampler;
		Mesh _mesh;
		VkDescriptorPool _descriptorPool;
		VkDescriptorSetLayout _descriptorLayout;
		jlb::Array<VkDescriptorSet> _descriptorSets{};
		jlb::Array<Buffer> _instanceBuffers{};

		VkPipelineLayout _pipelineLayout;
		VkPipeline _pipeline;

		void LoadTextureAtlas(const EngineData& EngineData, const CreateInfo& createInfo);
		void UnloadTextureAtlas(const EngineData& EngineData);

		void CreateMesh(const EngineData& EngineData);
		void CreateShaderAssets(const EngineData& EngineData);
		void DestroyShaderAssets(const EngineData& EngineData);
	};

	template <typename Task>
	void RenderSystem<Task>::CreateSwapChainAssets(const EngineData& EngineData, SystemChain& chain)
	{
		TaskSystem<Task>::CreateSwapChainAssets(EngineData, chain);

		jlb::StackArray<pipeline::Info::Module, 2> modules{};
		modules[0].module = _shader.vert;
		modules[0].flags = VK_SHADER_STAGE_VERTEX_BIT;
		modules[1].module = _shader.frag;
		modules[1].flags = VK_SHADER_STAGE_FRAGMENT_BIT;

		auto vertAttributes = Vertex::GetAttributeDescriptions();
		auto vertBindings = Vertex::GetBindingDescriptions();

		pipeline::Info pipelineInfo{};
		pipelineInfo.vertInputAttribDescriptions = vertAttributes;
		pipelineInfo.vertInputBindingDescriptions = vertBindings;
		pipelineInfo.resolution = EngineData.resolution;
		pipelineInfo.modules = modules;
		pipelineInfo.renderPass = EngineData.swapChainRenderPass;
		pipelineInfo.layouts = _descriptorLayout;
		pipelineInfo.pushConstantSize = sizeof(PushConstants);
		pipelineInfo.usePushConstant = true;

		pipeline::Create(EngineData, pipelineInfo, _pipelineLayout, _pipeline);
	}

	template <typename Task>
	void RenderSystem<Task>::DestroySwapChainAssets(const EngineData& EngineData, SystemChain& chain)
	{
		auto& logicalDevice = EngineData.app->logicalDevice;

		vkDestroyPipeline(logicalDevice, _pipeline, nullptr);
		vkDestroyPipelineLayout(logicalDevice, _pipelineLayout, nullptr);

		TaskSystem<Task>::DestroySwapChainAssets(EngineData, chain);
	}

	template <typename Task>
	const Texture& RenderSystem<Task>::GetTexture() const
	{
		return _textureAtlas;
	}

	template <typename Task>
	void RenderSystem<Task>::Update(const EngineData& EngineData, SystemChain& chain)
	{
		auto& cmd = EngineData.swapChainCommandBuffer;

		auto& logicalDevice = EngineData.app->logicalDevice;
		auto& memBlock = _instanceBuffers[EngineData.swapChainImageIndex].memBlock;
		void* instanceData;
		const auto result = vkMapMemory(logicalDevice, memBlock.memory, memBlock.offset, memBlock.size, 0, &instanceData);
		assert(!result);
		memcpy(instanceData, static_cast<const void*>(TaskSystem<Task>::GetData()), sizeof(Task) * TaskSystem<Task>::GetLength());
		vkUnmapMemory(logicalDevice, memBlock.memory);

		VkDeviceSize offset = 0;
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout,
			0, 1, &_descriptorSets[EngineData.swapChainImageIndex], 0, nullptr);
		vkCmdBindVertexBuffers(cmd, 0, 1, &_mesh.vertexBuffer.buffer, &offset);
		vkCmdBindIndexBuffer(cmd, _mesh.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

		auto& cameras = *chain.Get<CameraArchetype>();
		for (const auto& camera : cameras)
		{
			PushConstants pushConstant{};
			pushConstant.resolution = EngineData.resolution;
			pushConstant.camera = camera;

			vkCmdPushConstants(cmd, _pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &pushConstant);
			vkCmdDrawIndexed(cmd, _mesh.indexCount, TaskSystem<Task>::GetCount(), 0, 0, 0);
		}

		TaskSystem<Task>::SetCount(0);
	}

	template <typename Task>
	void RenderSystem<Task>::Allocate(const EngineData& EngineData, SystemChain& chain)
	{
		const CreateInfo createInfo = GetCreateInfo();

		TaskSystem<Task>::Allocate(EngineData, chain);
		LoadTextureAtlas(EngineData, createInfo);
		_shader = ShaderHandler::Create(EngineData, createInfo.vertPath, createInfo.fragPath);
		CreateMesh(EngineData);
		CreateShaderAssets(EngineData);
	}

	template <typename Task>
	void RenderSystem<Task>::Free(const EngineData& EngineData, SystemChain& chain)
	{
		DestroyShaderAssets(EngineData);
		mesh::Destroy(EngineData, _mesh);
		ShaderHandler::Destroy(EngineData, _shader);
		UnloadTextureAtlas(EngineData);
		TaskSystem<Task>::Free(EngineData, chain);
	}

	template <typename Task>
	void RenderSystem<Task>::LoadTextureAtlas(const EngineData& EngineData, const CreateInfo& createInfo)
	{
		auto& app = *EngineData.app;
		auto& logicalDevice = app.logicalDevice;

		_textureAtlas = texture::Load(EngineData, createInfo.atlasTexturePath);
		const auto viewCreateInfo = vk::image::CreateViewDefaultInfo(_textureAtlas.image, texture::GetFormat());
		auto result = vkCreateImageView(logicalDevice, &viewCreateInfo, nullptr, &_atlasImageView);
		assert(!result);
		const auto samplerCreateInfo = vk::sampler::CreateDefaultInfo(app);
		result = vkCreateSampler(logicalDevice, &samplerCreateInfo, nullptr, &_atlasSampler);
		assert(!result);
	}

	template <typename Task>
	void RenderSystem<Task>::UnloadTextureAtlas(const EngineData& EngineData)
	{
		auto& logicalDevice = EngineData.app->logicalDevice;

		vkDestroySampler(logicalDevice, _atlasSampler, nullptr);
		vkDestroyImageView(logicalDevice, _atlasImageView, nullptr);
		texture::Free(EngineData, _textureAtlas);
	}

	template <typename Task>
	void RenderSystem<Task>::CreateMesh(const EngineData& EngineData)
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

		for (auto& vertex : vertices)
			vertex.position /= 2;

		_mesh = mesh::CreateIndexed<Vertex, Vertex::Index>(EngineData, vertices, indices);
	}

	template <typename Task>
	void RenderSystem<Task>::CreateShaderAssets(const EngineData& EngineData)
	{
		auto& app = *EngineData.app;
		auto& allocator = *EngineData.allocator;
		auto& tempAllocator = *EngineData.tempAllocator;
		auto& logicalDevice = app.logicalDevice;
		const size_t swapChainImageCount = EngineData.swapChainImageCount;

		_instanceBuffers = instancing::CreateStorageBuffers<Task>(EngineData, TaskSystem<Task>::GetLength());

		// Create descriptor layout.
		jlb::StackArray<layout::Info::Binding, 2> bindings{};
		bindings[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		bindings[0].size = sizeof(Task) * TaskSystem<Task>::GetLength();
		bindings[0].flag = VK_SHADER_STAGE_VERTEX_BIT;
		bindings[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		bindings[1].flag = VK_SHADER_STAGE_FRAGMENT_BIT;

		layout::Info descriptorLayoutInfo{};
		descriptorLayoutInfo.bindings = bindings;
		_descriptorLayout = layout::Create(EngineData, descriptorLayoutInfo);

		// Create descriptor pool.
		VkDescriptorPoolSize poolSizes[2];
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes[0].descriptorCount = swapChainImageCount;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = swapChainImageCount;
		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = sizeof poolSizes / sizeof(VkDescriptorPoolSize);
		poolInfo.pPoolSizes = poolSizes;
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
			instanceInfo.buffer = _instanceBuffers[i].buffer;
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
			atlasInfo.imageLayout = texture::GetImageLayout();
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
	void RenderSystem<Task>::DestroyShaderAssets(const EngineData& EngineData)
	{
		auto& app = *EngineData.app;
		auto& logicalDevice = app.logicalDevice;
		auto& allocator = *EngineData.allocator;

		vkDestroyDescriptorPool(logicalDevice, _descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(logicalDevice, _descriptorLayout, nullptr);
		_descriptorSets.Free(*EngineData.allocator);

		for (int32_t i = EngineData.swapChainImageCount - 1; i >= 0; --i)
			FreeBuffer(EngineData, _instanceBuffers[i]);

		_instanceBuffers.Free(allocator);
	}
}
