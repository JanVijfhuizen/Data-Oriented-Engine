#pragma once
#include "VkRenderer/VkMemBlock.h"

namespace vk
{
	class SwapChain;
}

namespace vke
{
	class PostEffectHandler final
	{
	public:
		void Allocate(jlb::StackAllocator& allocator, vk::App& app, vk::SwapChain& swapChain, vk::StackAllocator& vkAllocator);
		void Free(jlb::StackAllocator& allocator, vk::App& app, vk::SwapChain& swapChain, vk::StackAllocator& vkAllocator);
		void RecreateFrames(jlb::StackAllocator& allocator, vk::App& app, vk::SwapChain& swapChain, vk::StackAllocator& vkAllocator);

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

		void FreeFrames(jlb::StackAllocator& allocator, vk::App& app, vk::SwapChain& swapChain, vk::StackAllocator& vkAllocator);
	};
}
