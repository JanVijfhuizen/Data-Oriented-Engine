#pragma once
#include "TaskSystem.h"
#include "VkEngine/Graphics/Camera.h"
#include "VkEngine/Graphics/Shader.h"
#include "VkEngine/Graphics/Mesh.h"
#include "VkEngine/Graphics/Texture.h"
#include "VkEngine/Graphics/ShaderLoader.h"
#include "StackArray.h"
#include "VkEngine/Graphics/Vertex.h"
#include "VkEngine/Graphics/MeshUtils.h"
#include "VkEngine/Graphics/TextureUtils.h"
#include "VkRenderer/VkImageUtils.h"
#include "VkRenderer/VkSamplerUtils.h"
#include "VkRenderer/VkApp.h"
#include "VkEngine/Graphics/BufferUtils.h"
#include "VkEngine/Graphics/LayoutUtils.h"
#include "VkEngine/Graphics/InstancingUtils.h"
#include "VkEngine/Graphics/PipelineUtils.h"

namespace vke
{
	template <typename Task>
	class RenderSystem : public TaskSystem<Task>
	{
	public:
		Camera camera{};

	protected:
		[[nodiscard]] virtual jlb::StringView GetTextureAtlasFilePath() const = 0;
		[[nodiscard]] virtual jlb::StringView GetFragmentShaderPath() const = 0;
		[[nodiscard]] virtual jlb::StringView GetVertexShaderPath() const = 0;
		virtual void DefineMeshShape(size_t& outVerticesLength, size_t& outIndicesLength);
		virtual void DefineMesh(jlb::ArrayView<Vertex> vertices, jlb::ArrayView<Vertex::Index> indices);

	private:
		struct PushConstants final
		{
			glm::vec2 resolution;
			Camera camera;
		};

		struct TextureAtlas final
		{
			Texture texture;
			VkImageView imageView;
			VkSampler sampler;
		};

		Shader _shader;
		Mesh _mesh;
		TextureAtlas _textureAtlas;
		jlb::Array<Buffer> _instanceBuffers;
		VkDescriptorSetLayout _descriptorLayout;
		VkDescriptorPool _descriptorPool;
		jlb::Array<VkDescriptorSet> _descriptorSets{};

		VkPipeline _pipeline;
		VkPipelineLayout _pipelineLayout;

		void Allocate(const EngineData& info) override;
		void Free(const EngineData& info) override;
		void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::NestableVector<Task>& tasks) override;
		void OnRecreateSwapChainAssets(const EngineData& info, jlb::Systems<EngineData> systems) override;
		
		void CreateShaderAssets(const EngineData& info);
		void DestroyShaderAssets(const EngineData& info);

		void CreateSwapChainAssets(const EngineData& info);
		void DestroySwapChainAssets(const EngineData& info) const;

		[[nodiscard]] size_t DefineNestedCapacity(const EngineData& info) override;
	};

	template<typename Task>
	void RenderSystem<Task>::DefineMeshShape(size_t& outVerticesLength, size_t& outIndicesLength)
	{
		outVerticesLength = 4;
		outIndicesLength = 6;
	}

	template<typename Task>
	void RenderSystem<Task>::DefineMesh(jlb::ArrayView<Vertex> vertices, jlb::ArrayView<Vertex::Index> indices)
	{
		vertices[0].position = { -1, -1 };
		vertices[1].position = { -1, 1 };
		vertices[2].position = { 1, 1 };
		vertices[3].position = { 1, -1 };

		vertices[0].textureCoordinates = { 0, 0 };
		vertices[1].textureCoordinates = { 0, 1 };
		vertices[2].textureCoordinates = { 1, 1 };
		vertices[3].textureCoordinates = { 1, 0 };

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 0;
		indices[4] = 2;
		indices[5] = 3;
	}

	template <typename Task>
	void RenderSystem<Task>::Allocate(const EngineData& info)
	{
		TaskSystem<Task>::Allocate(info);

		const auto& app = *info.app;
		const auto& logicalDevice = app.logicalDevice;

		_shader = shader::Load(info, GetVertexShaderPath(), GetFragmentShaderPath());

		// Load mesh.
		jlb::Array<Vertex> vertices{};
		jlb::Array<Vertex::Index> indices{};

		{
			size_t verticesLength;
			size_t indicesLength;

			DefineMeshShape(verticesLength, indicesLength);
			vertices.Allocate(*info.tempAllocator, verticesLength);
			indices.Allocate(*info.tempAllocator, indicesLength);
			DefineMesh(vertices, indices);
		}
		
		for (auto& vertex : vertices)
			vertex.position /= 2;
		_mesh = mesh::CreateIndexed<Vertex, Vertex::Index>(info, vertices, indices);

		indices.Free(*info.tempAllocator);
		vertices.Free(*info.tempAllocator);

		// Load texture.
		_textureAtlas.texture = texture::Load(info, GetTextureAtlasFilePath());
		const auto viewCreateInfo = vk::image::CreateViewDefaultInfo(_textureAtlas.texture.image, texture::DEFAULT_FORMAT);
		auto result = vkCreateImageView(logicalDevice, &viewCreateInfo, nullptr, &_textureAtlas.imageView);
		assert(!result);
		const auto samplerCreateInfo = vk::sampler::CreateDefaultInfo(app);
		result = vkCreateSampler(logicalDevice, &samplerCreateInfo, nullptr, &_textureAtlas.sampler);
		assert(!result);

		if (TaskSystem<Task>::DefineCapacity(info) == 0)
			return;

		CreateShaderAssets(info);
		CreateSwapChainAssets(info);
	}

	template <typename Task>
	void RenderSystem<Task>::Free(const EngineData& info)
	{
		if (TaskSystem<Task>::DefineCapacity(info) == 0)
		{
			DestroySwapChainAssets(info);
			DestroyShaderAssets(info);
		}

		const auto& logicalDevice = info.app->logicalDevice;

		vkDestroySampler(logicalDevice, _textureAtlas.sampler, nullptr);
		vkDestroyImageView(logicalDevice, _textureAtlas.imageView, nullptr);
		texture::Free(info, _textureAtlas.texture);
		mesh::Destroy(info, _mesh);
		shader::Unload(info, _shader);

		TaskSystem<Task>::Free(info);
	}

	template <typename Task>
	void RenderSystem<Task>::OnRecreateSwapChainAssets(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		TaskSystem<Task>::OnRecreateSwapChainAssets(info, systems);
		DestroySwapChainAssets(info);
		CreateSwapChainAssets(info);
	}

	template <typename Task>
	void RenderSystem<Task>::CreateShaderAssets(const EngineData& info)
	{
		const auto& app = *info.app;
		auto& allocator = *info.allocator;
		auto& tempAllocator = *info.tempAllocator;
		const auto& logicalDevice = app.logicalDevice;
		const size_t swapChainImageCount = info.swapChainData->imageCount;

		const size_t capacity = TaskSystem<Task>::DefineCapacity(info);
		_instanceBuffers = instancing::CreateStorageBuffers<Task>(info, capacity);

		// Create descriptor layout.
		jlb::StackArray<layout::Info::Binding, 2> bindings{};
		bindings[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		bindings[0].size = sizeof(Task) * capacity;
		bindings[0].flag = VK_SHADER_STAGE_VERTEX_BIT;
		bindings[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		bindings[1].flag = VK_SHADER_STAGE_FRAGMENT_BIT;

		layout::Info descriptorLayoutInfo{};
		descriptorLayoutInfo.bindings = bindings;
		_descriptorLayout = layout::Create(info, descriptorLayoutInfo);

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
			instanceInfo.range = sizeof(Task) * capacity;

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
			atlasInfo.imageLayout = texture::DEFAULT_LAYOUT;
			atlasInfo.imageView = _textureAtlas.imageView;
			atlasInfo.sampler = _textureAtlas.sampler;

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
	void RenderSystem<Task>::DestroyShaderAssets(const EngineData& info)
	{
		const auto& app = *info.app;
		const auto& logicalDevice = app.logicalDevice;
		auto& allocator = *info.allocator;

		vkDestroyDescriptorPool(logicalDevice, _descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(logicalDevice, _descriptorLayout, nullptr);
		_descriptorSets.Free(*info.allocator);

		for (int32_t i = info.swapChainData->imageCount - 1; i >= 0; --i)
			buffer::Free(info, _instanceBuffers[i]);

		_instanceBuffers.Free(allocator);
	}

	template <typename Task>
	void RenderSystem<Task>::CreateSwapChainAssets(const EngineData& info)
	{
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
		pipelineInfo.resolution = info.swapChainData->resolution;
		pipelineInfo.modules = modules;
		pipelineInfo.renderPass = info.swapChainData->renderPass;
		pipelineInfo.layouts = _descriptorLayout;
		pipelineInfo.pushConstantSize = sizeof(PushConstants);
		pipelineInfo.usePushConstant = true;

		pipeline::Create(info, pipelineInfo, _pipelineLayout, _pipeline);
	}

	template <typename Task>
	void RenderSystem<Task>::DestroySwapChainAssets(const EngineData& info) const
	{
		const auto& logicalDevice = info.app->logicalDevice;

		vkDestroyPipeline(logicalDevice, _pipeline, nullptr);
		vkDestroyPipelineLayout(logicalDevice, _pipelineLayout, nullptr);
	}

	template <typename Task>
	size_t RenderSystem<Task>::DefineNestedCapacity(const EngineData& info)
	{
		return 0;
	}

	template <typename Task>
	void RenderSystem<Task>::OnUpdate(const EngineData& info,
		const jlb::Systems<EngineData> systems,
		const jlb::NestableVector<Task>& tasks)
	{
		const auto& root = tasks.GetRoot();

		const auto& cmd = info.swapChainData->commandBuffer;

		const auto& logicalDevice = info.app->logicalDevice;
		const auto& memBlock = _instanceBuffers[info.swapChainData->imageIndex].memBlock;
		void* instanceData;
		const auto result = vkMapMemory(logicalDevice, memBlock.memory, memBlock.offset, memBlock.size, 0, &instanceData);
		assert(!result);
		memcpy(instanceData, static_cast<const void*>(root.GetData()), sizeof(Task) * root.GetCount());
		vkUnmapMemory(logicalDevice, memBlock.memory);

		VkDeviceSize offset = 0;
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout,
			0, 1, &_descriptorSets[info.swapChainData->imageIndex], 0, nullptr);
		vkCmdBindVertexBuffers(cmd, 0, 1, &_mesh.vertexBuffer.buffer, &offset);
		vkCmdBindIndexBuffer(cmd, _mesh.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

		PushConstants pushConstant{};
		pushConstant.resolution = info.swapChainData->resolution;
		pushConstant.camera = camera;

		vkCmdPushConstants(cmd, _pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &pushConstant);
		vkCmdDrawIndexed(cmd, _mesh.indexCount, root.GetCount(), 0, 0, 0);
	}
}
