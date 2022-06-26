#pragma once
#include "TaskSystem.h"
#include "VkEngine/Components/Transform.h"
#include "VkEngine/Graphics/Camera.h"
#include "VkEngine/Graphics/Shader.h"
#include "VkEngine/Graphics/Mesh.h"
#include "VkEngine/Graphics/Texture.h"

namespace vke
{
	struct EntityRenderTask final
	{
		Transform transform{};
	};

	class EntityRenderSystem final : public TaskSystem<EntityRenderTask>
	{
	public:
		Camera camera{};

	private:
		struct PushConstants final
		{
			glm::vec2 resolution;
			Camera camera;
		};

		struct TextureAtlas final
		{
			Texture texture;
			VkImageView imageView;
			VkSampler sampler;
		};

		Shader _shader;
		Mesh _mesh;
		TextureAtlas _textureAtlas;
		jlb::Array<Buffer> _instanceBuffers;
		VkDescriptorSetLayout _descriptorLayout;
		VkDescriptorPool _descriptorPool;
		jlb::Array<VkDescriptorSet> _descriptorSets{};

		VkPipeline _pipeline;
		VkPipelineLayout _pipelineLayout;

		void Allocate(const EngineData& info) override;
		void Free(const EngineData& info) override;
		void Awake(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void Exit(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::Vector<EntityRenderTask>& tasks) override;
		void OnRecreateSwapChainAssets(const EngineData& info, jlb::Systems<EngineData> systems) override;

		void CreateShaderAssets(const EngineData& info);
		void DestroyShaderAssets(const EngineData& info);

		void CreateSwapChainAssets(const EngineData& info);
		void DestroySwapChainAssets(const EngineData& info) const;
	};
}
