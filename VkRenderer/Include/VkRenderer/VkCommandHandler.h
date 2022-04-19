#pragma once
#include "ArrayView.h"

namespace vk
{
	struct App;

	class CommandHandler final
	{
	public:
		[[nodiscard]] static VkCommandBufferAllocateInfo CreateBufferDefaultInfo(App& app);
		[[nodiscard]] static VkCommandBufferBeginInfo CreateBufferBeginDefaultInfo();
		[[nodiscard]] static VkSubmitInfo CreateSubmitDefaultInfo(jlb::ArrayView<VkCommandBuffer> commandBuffers);
	};
}
