#include "VkEngine/pch.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "StringView.h"
#include <VkEngine/Graphics/ShaderLoader.h>
#include "VkEngine/Graphics/MeshUtils.h"
#include "StackArray.h"
#include "VkEngine/Graphics/Vertex.h"
#include "VkEngine/Graphics/TextureUtils.h"
#include "VkRenderer/VkImageUtils.h"
#include "VkRenderer/VkSamplerUtils.h"

namespace vke
{
	void EntityRenderSystem::Allocate(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		auto& app = *info.app;
		auto& logicalDevice = app.logicalDevice;

		_shader = shader::Load(info, "Shaders/vert.spv", "Shaders/frag.spv");

		// Load mesh.
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
		_mesh = mesh::CreateIndexed<Vertex, Vertex::Index>(info, vertices, indices);

		// Load texture.
		_textureAtlas.texture = texture::Load(info, "Textures/Atlas.png");
		const auto viewCreateInfo = vk::image::CreateViewDefaultInfo(_textureAtlas.texture.image, texture::DEFAULT_FORMAT);
		auto result = vkCreateImageView(logicalDevice, &viewCreateInfo, nullptr, &_textureAtlas.imageView);
		assert(!result);
		const auto samplerCreateInfo = vk::sampler::CreateDefaultInfo(app);
		result = vkCreateSampler(logicalDevice, &samplerCreateInfo, nullptr, &_textureAtlas.sampler);
		assert(!result);

		CreateSwapChainAssets(info);
	}

	void EntityRenderSystem::Free(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		auto& logicalDevice = info.app->logicalDevice;

		vkDestroySampler(logicalDevice, _textureAtlas.sampler, nullptr);
		vkDestroyImageView(logicalDevice, _textureAtlas.imageView, nullptr);
		texture::Free(info, _textureAtlas.texture);
		mesh::Destroy(info, _mesh);
		DestroySwapChainAssets(info);
		shader::Unload(info, _shader);
	}

	void EntityRenderSystem::OnRecreateSwapChainAssets(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		GameSystem::OnRecreateSwapChainAssets(info, systems);
		DestroySwapChainAssets(info);
		CreateSwapChainAssets(info);
	}

	void EntityRenderSystem::CreateSwapChainAssets(const EngineData& info)
	{
		
	}

	void EntityRenderSystem::DestroySwapChainAssets(const EngineData& info)
	{
	}

	void EntityRenderSystem::OnUpdate(const EngineData& info, 
		const jlb::Systems<EngineData> systems,
		const jlb::Vector<EntityRenderTask>& tasks)
	{

	}
}
