#include "pch.h"
#include "Systems/DebugRenderSystem.h"
#include "Handlers/ShaderHandler.h"
#include "StringView.h"

namespace game
{
	void DebugRenderSystem::Allocate(const EngineOutData& outData, SystemChain& chain)
	{
		TaskSystem<DebugRenderTask>::Allocate(outData, chain);
		_shader = ShaderHandler::Create(outData, "Shaders/debug-vert.spv","Shaders/debug-frag.spv");
	}

	void DebugRenderSystem::Free(const EngineOutData& outData, SystemChain& chain)
	{
		ShaderHandler::Destroy(outData, _shader);
		TaskSystem<DebugRenderTask>::Free(outData, chain);
	}

	void DebugRenderSystem::Update(const EngineOutData& outData, SystemChain& chain)
	{
		
	}

	void DebugRenderSystem::CreateSwapChainAssets(const EngineOutData& outData, SystemChain& chain)
	{
		TaskSystem<DebugRenderTask>::CreateSwapChainAssets(outData, chain);
	}

	void DebugRenderSystem::DestroySwapChainAssets(const EngineOutData& outData, SystemChain& chain)
	{
		TaskSystem<DebugRenderTask>::DestroySwapChainAssets(outData, chain);
	}
}
