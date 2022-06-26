#pragma once
#include "TaskSystem.h"
#include "VkEngine/Components/Transform.h"
#include "VkEngine/Components/Camera.h"
#include "VkEngine/Graphics/Shader.h"
#include "VkEngine/Graphics/Mesh.h"

namespace vke
{
	struct EntityRenderTask final
	{
		Transform transform{};
	};

	class EntityRenderSystem final : public TaskSystem<EntityRenderTask>
	{
		struct PushConstants final
		{
			glm::vec2 resolution;
			Camera camera;
		};

		Shader _shader{};
		Mesh _mesh;

		void Allocate(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void Free(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::Vector<EntityRenderTask>& tasks) override;
		void OnRecreateSwapChainAssets(const EngineData& info, jlb::Systems<EngineData> systems) override;

		void CreateSwapChainAssets(const EngineData& info);
		void DestroySwapChainAssets(const EngineData& info);
	};
}
