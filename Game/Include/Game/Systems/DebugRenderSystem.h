#pragma once
#include "TaskSystem.h"
#include "Graphics/Shader.h"

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
		Shader _shader;

		void Allocate(const EngineOutData& outData, SystemChain& chain) override;
		void Free(const EngineOutData& outData, SystemChain& chain) override;
		void Update(const EngineOutData& outData, SystemChain& chain) override;

		void CreateSwapChainAssets(const EngineOutData& outData, SystemChain& chain) override;
		void DestroySwapChainAssets(const EngineOutData& outData, SystemChain& chain) override;
	};
}
