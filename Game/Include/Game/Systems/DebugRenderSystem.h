#pragma once
#include "TaskSystem.h"
#include "Graphics/Shader.h"
#include "Graphics/PipelineHandler.h"

namespace game
{
	struct DebugRenderTask final
	{
		// Line vertices.
		glm::vec2 positions[2];
		glm::vec3 color;
	};

	class DebugRenderSystem final : public TaskSystem<DebugRenderTask>
	{
	private:
		struct PushConstants final
		{
			glm::vec2 resolution;
			glm::vec2 cameraPosition;
			float pixelSize;
		};

		Shader _shader;
		VkPipeline _pipeline;
		VkPipelineLayout _pipelineLayout;
		VkDescriptorSetLayout _descriptorLayout;

		void Allocate(const EngineOutData& outData, SystemChain& chain) override;
		void Free(const EngineOutData& outData, SystemChain& chain) override;
		void Update(const EngineOutData& outData, SystemChain& chain) override;

		void CreateShaderAssets(const EngineOutData& outData);
		void DestroyShaderAssets(const EngineOutData& outData);

		void CreateSwapChainAssets(const EngineOutData& outData, SystemChain& chain) override;
		void DestroySwapChainAssets(const EngineOutData& outData, SystemChain& chain) override;
	};
}
