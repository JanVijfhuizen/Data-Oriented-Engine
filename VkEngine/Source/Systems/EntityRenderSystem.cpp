#include "VkEngine/pch.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "StringView.h"
#include <VkEngine/Graphics/ShaderLoader.h>
#include "VkEngine/Graphics/MeshUtils.h"
#include "StackArray.h"
#include "VkEngine/Graphics/Vertex.h"

namespace vke
{
	void EntityRenderSystem::Allocate(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		_shader = shader::Load(info, "Shaders/vert.spv", "Shaders/frag.spv");
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
		_mesh = mesh::CreateIndexed<Vertex, Vertex::Index>(info, vertices, indices);
		CreateSwapChainAssets(info);
	}

	void EntityRenderSystem::Free(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
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
