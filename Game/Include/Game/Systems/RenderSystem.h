#pragma once
#include "TaskSystem.h"
#include "Graphics/Mesh.h"
#include "Graphics/Vertex.h"

namespace game
{
	struct Light;
	struct Transform;
	struct Renderer;

	struct RenderTask final
	{
		Vertex::Instance vertexInstance{};
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
		Mesh _mesh;
		VkBuffer _instanceBuffer;
		vk::MemBlock _instanceMemBlock;

		VkPipelineLayout _pipelineLayout;
		VkPipeline _pipeline;

		void LoadShader(const EngineOutData& engineOutData);
		void UnloadShader(const EngineOutData& engineOutData) const;

		void CreateMesh(const EngineOutData& engineOutData);
		void CreateInstanceArray(const EngineOutData& engineOutData);
		void DestroyInstanceArray(const EngineOutData& engineOutData);
	};
}
