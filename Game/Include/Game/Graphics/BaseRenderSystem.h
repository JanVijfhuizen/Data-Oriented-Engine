#pragma once
#include "StringView.h"
#include "VkRenderer/VkMemBlock.h"
#include "Array.h"
#include "Mesh.h"
#include "Texture.h"

namespace game
{
	class BaseRenderSystem
	{
	public:
		struct CreateInfo final
		{
			jlb::StringView texturePath{};
			jlb::StringView shaderFragPath{};
			jlb::StringView shaderVertPath{};
			size_t instanceSize;
			size_t instanceCount;
			jlb::ArrayView<VkVertexInputAttributeDescription> vertInputAttribDescriptions{};
			jlb::ArrayView<VkVertexInputBindingDescription> vertInputBindingDescriptions{};
		};

		virtual void Allocate(EngineOutData& engineOutData, CreateInfo& info);
		virtual void Free(EngineOutData& engineOutData);

		void Update(EngineOutData& engineOutData, void* instanceSrc, size_t instanceSize, size_t instanceCount);

		void CreateSwapChainAssets(const EngineOutData& engineOutData);
		void DestroySwapChainAssets(const EngineOutData& engineOutData) const;

	protected:
		[[nodiscard]] virtual Mesh CreateMesh(const EngineOutData& engineOutData) = 0;

	private:
		VkShaderModule _vertModule;
		VkShaderModule _fragModule;
		Texture _textureAtlas;
		VkImageView _atlasImageView;
		VkSampler _atlasSampler;
		Mesh _mesh;
		VkDescriptorPool _descriptorPool;
		VkDescriptorSetLayout _descriptorLayout;
		jlb::Array<VkDescriptorSet> _descriptorSets{};
		jlb::Array<VkBuffer> _instanceBuffers{};
		jlb::Array<vk::MemBlock> _instanceMemBlocks{};

		VkPipelineLayout _pipelineLayout;
		VkPipeline _pipeline;

		void LoadShader(const EngineOutData& engineOutData, jlb::StringView vertPath, jlb::StringView fragPath);
		void UnloadShader(const EngineOutData& engineOutData) const;
		void LoadTextureAtlas(const EngineOutData& engineOutData, jlb::StringView path);
		void UnloadTextureAtlas(const EngineOutData& engineOutData);

		void CreateShaderAssets(const EngineOutData& engineOutData, size_t instanceSize, size_t instanceCount);
		void DestroyShaderAssets(const EngineOutData& engineOutData);
	};
}
