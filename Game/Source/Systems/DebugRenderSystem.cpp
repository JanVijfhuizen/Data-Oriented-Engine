#include "pch.h"
#include "Systems/DebugRenderSystem.h"
#include "Handlers/ShaderHandler.h"
#include "StringView.h"
#include "StackArray.h"
#include "Graphics/PipelineHandler.h"
#include "VkRenderer/VkApp.h"

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

	void DebugRenderSystem::CreateShaderAssets(const EngineOutData& outData)
	{
	}

	void DebugRenderSystem::DestroyShaderAssets(const EngineOutData& outData)
	{
	}

	void DebugRenderSystem::CreateSwapChainAssets(const EngineOutData& outData, SystemChain& chain)
	{
		return;
		TaskSystem<DebugRenderTask>::CreateSwapChainAssets(outData, chain);
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
		attributeDescription.format = VK_FORMAT_R32G32_SINT;
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

		PipelineHandler::Create(outData, pipelineInfo, _pipelineLayout, _pipeline);
	}

	void DebugRenderSystem::DestroySwapChainAssets(const EngineOutData& outData, SystemChain& chain)
	{
		return;
		auto& logicalDevice = outData.app->logicalDevice;

		vkDestroyPipeline(logicalDevice, _pipeline, nullptr);
		vkDestroyPipelineLayout(logicalDevice, _pipelineLayout, nullptr);

		TaskSystem<DebugRenderTask>::DestroySwapChainAssets(outData, chain);
	}
}
