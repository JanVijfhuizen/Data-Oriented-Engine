#pragma once
#include "TaskSystem.h"
#include "Graphics/Mesh.h"
#include "Components/Transform.h"
#include "Graphics/Texture.h"

namespace game
{
	struct Light;
	struct Renderer;

	struct RenderTask final
	{
		Transform transform{};
	};

	class RenderSystem final : public jlb::TaskSystem<RenderTask>
	{
	public:
		void Allocate(const EngineOutData& engineOutData);
		void Free(const EngineOutData& engineOutData);

		void Update(const EngineOutData& engineOutData);

		[[nodiscard]] static RenderTask CreateDefaultTask(Renderer& renderer, Transform& transform);

		void CreateSwapChainAssets(const EngineOutData& engineOutData);
		void DestroySwapChainAssets(const EngineOutData& engineOutData) const;

	private:
		using TaskSystem<RenderTask>::Allocate;
		using TaskSystem<RenderTask>::Free;

		VkShaderModule _vertModule;
		VkShaderModule _fragModule;
		Texture _textureAtlas;
		Mesh _mesh;
		VkDescriptorPool _descriptorPool;
		VkDescriptorSetLayout _descriptorLayout;
		jlb::Array<VkDescriptorSet> _descriptorSets{};
		jlb::Array<VkBuffer> _instanceBuffers{};
		jlb::Array<vk::MemBlock> _instanceMemBlocks{};

		VkPipelineLayout _pipelineLayout;
		VkPipeline _pipeline;

		void LoadShader(const EngineOutData& engineOutData);
		void UnloadShader(const EngineOutData& engineOutData) const;

		void CreateMesh(const EngineOutData& engineOutData);
		void CreateShaderAssets(const EngineOutData& engineOutData);
		void DestroyShaderAssets(const EngineOutData& engineOutData);
	};
}
