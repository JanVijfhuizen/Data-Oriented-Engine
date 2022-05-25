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
#include "Graphics/Shader.h"
#include "Handlers/ShaderHandler.h"
#include "Graphics/InstanceUtils.h"
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

		void Allocate(const EngineOutData& outData, SystemChain& chain) override;
		void Free(const EngineOutData& outData, SystemChain& chain) override;
		void Update(const EngineOutData& outData, SystemChain& chain) override;

		void CreateSwapChainAssets(const EngineOutData& outData, SystemChain& chain) override;
		void DestroySwapChainAssets(const EngineOutData& outData, SystemChain& chain) override;

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

		void LoadTextureAtlas(const EngineOutData& outData, const CreateInfo& createInfo);
		void UnloadTextureAtlas(const EngineOutData& outData);

		void CreateMesh(const EngineOutData& outData);
		void CreateShaderAssets(const EngineOutData& outData);
		void DestroyShaderAssets(const EngineOutData& outData);
	};

	template <typename Task>
	void RenderSystem<Task>::CreateSwapChainAssets(const EngineOutData& outData, SystemChain& chain)
	{
		TaskSystem<Task>::CreateSwapChainAssets(outData, chain);

		jlb::StackArray<PipelineHandler::Info::Module, 2> modules{};
		modules[0].module = _shader.vert;
		modules[0].flags = VK_SHADER_STAGE_VERTEX_BIT;
		modules[1].module = _shader.frag;
		modules[1].flags = VK_SHADER_STAGE_FRAGMENT_BIT;

		auto vertAttributes = Vertex::GetAttributeDescriptions();
		auto vertBindings = Vertex::GetBindingDescriptions();

		PipelineHandler::Info pipelineInfo{};
		pipelineInfo.vertInputAttribDescriptions = vertAttributes;
		pipelineInfo.vertInputBindingDescriptions = vertBindings;
		pipelineInfo.resolution = outData.resolution;
		pipelineInfo.modules = modules;
		pipelineInfo.renderPass = outData.swapChainRenderPass;
		pipelineInfo.layouts = _descriptorLayout;
		pipelineInfo.pushConstantSize = sizeof(PushConstants);
		pipelineInfo.usePushConstant = true;

		PipelineHandler::Create(outData, pipelineInfo, _pipelineLayout, _pipeline);
	}

	template <typename Task>
	void RenderSystem<Task>::DestroySwapChainAssets(const EngineOutData& outData, SystemChain& chain)
	{
		auto& logicalDevice = outData.app->logicalDevice;

		vkDestroyPipeline(logicalDevice, _pipeline, nullptr);
		vkDestroyPipelineLayout(logicalDevice, _pipelineLayout, nullptr);

		TaskSystem<Task>::DestroySwapChainAssets(outData, chain);
	}

	template <typename Task>
	const Texture& RenderSystem<Task>::GetTexture() const
	{
		return _textureAtlas;
	}

	template <typename Task>
	void RenderSystem<Task>::Update(const EngineOutData& outData, SystemChain& chain)
	{
		auto& cameras = *chain.Get<CameraArchetype>();

		if (cameras.GetCount())
		{
			auto& cmd = outData.swapChainCommandBuffer;

			auto& logicalDevice = outData.app->logicalDevice;
			auto& memBlock = _instanceBuffers[outData.swapChainImageIndex].memBlock;
			void* instanceData;
			const auto result = vkMapMemory(logicalDevice, memBlock.memory, memBlock.offset, memBlock.size, 0, &instanceData);
			assert(!result);
			memcpy(instanceData, static_cast<const void*>(TaskSystem<Task>::GetData()), sizeof(Task) * TaskSystem<Task>::GetLength());
			vkUnmapMemory(logicalDevice, memBlock.memory);

			VkDeviceSize offset = 0;
			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout,
				0, 1, &_descriptorSets[outData.swapChainImageIndex], 0, nullptr);
			vkCmdBindVertexBuffers(cmd, 0, 1, &_mesh.vertexBuffer.buffer, &offset);
			vkCmdBindIndexBuffer(cmd, _mesh.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

			PushConstants pushConstant{};
			pushConstant.resolution = outData.resolution;
			pushConstant.camera = cameras[0];

			vkCmdPushConstants(cmd, _pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &pushConstant);
			vkCmdDrawIndexed(cmd, _mesh.indexCount, TaskSystem<Task>::GetCount(), 0, 0, 0);
		}
		TaskSystem<Task>::SetCount(0);
	}

	template <typename Task>
	void RenderSystem<Task>::Allocate(const EngineOutData& outData, SystemChain& chain)
	{
		const CreateInfo createInfo = GetCreateInfo();

		TaskSystem<Task>::Allocate(outData, chain);
		LoadTextureAtlas(outData, createInfo);
		_shader = ShaderHandler::Create(outData, createInfo.vertPath, createInfo.fragPath);
		CreateMesh(outData);
		CreateShaderAssets(outData);
	}

	template <typename Task>
	void RenderSystem<Task>::Free(const EngineOutData& outData, SystemChain& chain)
	{
		DestroyShaderAssets(outData);
		MeshHandler::Destroy(outData, _mesh);
		ShaderHandler::Destroy(outData, _shader);
		UnloadTextureAtlas(outData);
		TaskSystem<Task>::Free(outData, chain);
	}

	template <typename Task>
	void RenderSystem<Task>::LoadTextureAtlas(const EngineOutData& outData, const CreateInfo& createInfo)
	{
		auto& app = *outData.app;
		auto& logicalDevice = app.logicalDevice;

		_textureAtlas = TextureHandler::LoadTexture(outData, createInfo.atlasTexturePath);
		const auto viewCreateInfo = vk::ImageHandler::CreateViewDefaultInfo(_textureAtlas.image, TextureHandler::GetTextureFormat());
		auto result = vkCreateImageView(logicalDevice, &viewCreateInfo, nullptr, &_atlasImageView);
		assert(!result);
		const auto samplerCreateInfo = vk::SamplerHandler::CreateDefaultInfo(app);
		result = vkCreateSampler(logicalDevice, &samplerCreateInfo, nullptr, &_atlasSampler);
		assert(!result);
	}

	template <typename Task>
	void RenderSystem<Task>::UnloadTextureAtlas(const EngineOutData& outData)
	{
		auto& logicalDevice = outData.app->logicalDevice;

		vkDestroySampler(logicalDevice, _atlasSampler, nullptr);
		vkDestroyImageView(logicalDevice, _atlasImageView, nullptr);
		TextureHandler::FreeTexture(outData, _textureAtlas);
	}

	template <typename Task>
	void RenderSystem<Task>::CreateMesh(const EngineOutData& outData)
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

		_mesh = MeshHandler::CreateIndexed<Vertex, Vertex::Index>(outData, vertices, indices);
	}

	template <typename Task>
	void RenderSystem<Task>::CreateShaderAssets(const EngineOutData& outData)
	{
		auto& app = *outData.app;
		auto& allocator = *outData.allocator;
		auto& tempAllocator = *outData.tempAllocator;
		auto& logicalDevice = app.logicalDevice;
		const size_t swapChainImageCount = outData.swapChainImageCount;

		_instanceBuffers = CreateInstanceStorageBuffers<Task>(outData, TaskSystem<Task>::GetLength());

		// Create descriptor layout.
		jlb::StackArray<LayoutHandler::Info::Binding, 2> bindings{};
		bindings[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		bindings[0].size = sizeof(Task) * TaskSystem<Task>::GetLength();
		bindings[0].flag = VK_SHADER_STAGE_VERTEX_BIT;
		bindings[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		bindings[1].flag = VK_SHADER_STAGE_FRAGMENT_BIT;

		LayoutHandler::Info descriptorLayoutInfo{};
		descriptorLayoutInfo.bindings = bindings;
		_descriptorLayout = LayoutHandler::Create(outData, descriptorLayoutInfo);

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
	void RenderSystem<Task>::DestroyShaderAssets(const EngineOutData& outData)
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
}
