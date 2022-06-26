#include "VkEngine/pch.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "StringView.h"
#include <VkEngine/Graphics/ShaderLoader.h>

namespace vke
{
	void EntityRenderSystem::Allocate(const EngineData& info, jlb::Systems<EngineData> systems)
	{
		_shader = shader::Load(info, "Shaders/vert.spv", "Shaders/frag.spv");
		CreateSwapChainAssets(info);
	}

	void EntityRenderSystem::Free(const EngineData& info, jlb::Systems<EngineData> systems)
	{
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
