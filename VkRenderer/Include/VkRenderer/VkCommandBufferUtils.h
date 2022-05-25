#pragma once
#include "ArrayView.h"

namespace vk
{
	struct App;

	namespace cmdBuffer
	{
		[[nodiscard]] VkCommandBufferAllocateInfo CreateDefaultInfo(App& app);
		[[nodiscard]] VkCommandBufferBeginInfo CreateBeginDefaultInfo();
		[[nodiscard]] VkSubmitInfo CreateSubmitDefaultInfo(jlb::ArrayView<VkCommandBuffer> commandBuffers);
	}
}
