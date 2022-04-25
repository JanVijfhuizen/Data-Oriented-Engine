#pragma once
#include "TaskSystem.h"
#include "Graphics/Mesh.h"
#include "Graphics/Vertex.h"
#include "Components/Transform.h"

namespace game
{
	struct Light;
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

		void SetCameraTransform(Transform& transform);

		void CreateSwapChainAssets(const EngineOutData& engineOutData);
		void DestroySwapChainAssets(const EngineOutData& engineOutData) const;

	private:
		struct LayoutData final
		{
			Transform cameraTransform{};
		} _layoutData{};

		using TaskSystem<RenderTask>::Allocate;
		using TaskSystem<RenderTask>::Free;

		VkShaderModule _vertModule;
		VkShaderModule _fragModule;
		Mesh _mesh;
		VkBuffer _instanceBuffer;
		vk::MemBlock _instanceMemBlock;
		VkBuffer _layoutDataBuffer;
		vk::MemBlock _layoutDataMemBlock;

		VkPipelineLayout _pipelineLayout;
		VkPipeline _pipeline;
		VkDescriptorSetLayout _descriptorLayout;

		void LoadShader(const EngineOutData& engineOutData);
		void UnloadShader(const EngineOutData& engineOutData) const;

		void CreateMesh(const EngineOutData& engineOutData);
		void CreateBuffers(const EngineOutData& engineOutData);
		void DestroyBuffers(const EngineOutData& engineOutData);

		static void CreateUboArray(const EngineOutData& engineOutData, size_t size, vk::MemBlock& outMemBlock, VkBuffer& outBuffer);
		static void UpdateUboArray(const EngineOutData& engineOutData, size_t size, void* inData, vk::MemBlock& memBlock);
	};
}
