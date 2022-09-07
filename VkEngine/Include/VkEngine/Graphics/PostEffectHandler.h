#pragma once
#include "Shader.h"
#include "VkRenderer/VkMemBlock.h"

namespace vk
{
	class SwapChain;
}

namespace vke
{
	class PostEffectHandler;

	class PostEffect final
	{
		friend PostEffectHandler;

	public:
		void Allocate(const vk::App& app, PostEffectHandler& handler, const Shader& shader);
		void Free(const vk::App& app, PostEffectHandler& handler);
		void Recreate(const vk::App& app, PostEffectHandler& handler);

	private:
		VkCommandBuffer _commandBuffer = VK_NULL_HANDLE;
		VkSemaphore _renderFinishedSemaphore = VK_NULL_HANDLE;

		Shader _shader{};
		VkPipeline _pipeline = VK_NULL_HANDLE;
		VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
	};

	class PostEffectHandler final
	{
	public:
		void Allocate(const EngineData& info, vk::SwapChain& swapChain, vk::StackAllocator& vkAllocator);
		void Free(jlb::StackAllocator& allocator, const vk::App& app, vk::SwapChain& swapChain, const vk::StackAllocator& vkAllocator);
		void RecreateFrames(jlb::StackAllocator& allocator, const vk::App& app, vk::SwapChain& swapChain, vk::StackAllocator& vkAllocator);

	private:
		struct Frame final
		{
			struct Image final
			{
				VkImage image;
				VkImageView view;
				VkSampler sampler;
				vk::MemBlock memBlock;
			};

			Image images[2]{};
		};

		jlb::Array<Frame> _frames{};
		VkRenderPass _renderPass = VK_NULL_HANDLE;
		VkDescriptorSetLayout _descriptorLayout = VK_NULL_HANDLE;
		VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;

		void FreeFrames(jlb::StackAllocator& allocator, const vk::App& app, vk::SwapChain& swapChain, const vk::StackAllocator& vkAllocator);
	};
}
