#pragma once
#include "EntitySystem.h"

namespace game
{
	struct Light;
	struct Transform;
	struct Renderer;

	struct RenderTask final
	{
		
	};

	class RenderSystem final : public jlb::EntitySystem<RenderTask>
	{
	public:
		void Allocate(const EngineOutData& engineOutData, size_t size);
		void Free(const EngineOutData& engineOutData);

		void Update(const EngineOutData& engineOutData);

		[[nodiscard]] static RenderTask CreateDefaultTask(Renderer& renderer, Transform& transform);

		void CreateSwapChainAssets(const EngineOutData& engineOutData);
		void DestroySwapChainAssets(const EngineOutData& engineOutData) const;

	private:
		using EntitySystem<RenderTask>::Allocate;
		using EntitySystem<RenderTask>::AllocateAndCopy;
		using EntitySystem<RenderTask>::Free;

		VkShaderModule _vertModule;
		VkShaderModule _fragModule;

		VkPipelineLayout _pipelineLayout;
		VkPipeline _pipeline;

		void LoadShader(const EngineOutData& engineOutData);
		void UnloadShader(const EngineOutData& engineOutData) const;
	};
}
