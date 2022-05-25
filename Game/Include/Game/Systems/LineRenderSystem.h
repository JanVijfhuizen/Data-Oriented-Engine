#pragma once
#include "TaskSystem.h"
#include "Graphics/Shader.h"
#include "Graphics/Buffer.h"
#include "Archetypes/CameraArchetype.h"

namespace game
{
	struct LineRenderTask final
	{
		glm::vec2 start{};
		glm::vec2 end{};
	};

	class LineRenderSystem final : public TaskSystem<LineRenderTask>
	{
	private:
		struct PushConstants final
		{
			glm::vec2 resolution;
			Camera camera;
		};

		Shader _shader;
		Buffer _vertexBuffer;
		jlb::Array<Buffer> _instanceBuffers;

		VkPipeline _pipeline;
		VkPipelineLayout _pipelineLayout;
		VkDescriptorSetLayout _descriptorLayout;
		VkDescriptorPool _descriptorPool;
		jlb::Array<VkDescriptorSet> _descriptorSets{};

		void Allocate(const EngineOutData& outData, SystemChain& chain) override;
		void Free(const EngineOutData& outData, SystemChain& chain) override;
		void Update(const EngineOutData& outData, SystemChain& chain) override;

		void CreateShaderAssets(const EngineOutData& outData);
		void DestroyShaderAssets(const EngineOutData& outData);

		void CreateSwapChainAssets(const EngineOutData& outData, SystemChain& chain) override;
		void DestroySwapChainAssets(const EngineOutData& outData, SystemChain& chain) override;
	};
}
