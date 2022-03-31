#pragma once

namespace vk
{
	struct App;

	class CommandHandler final
	{
	public:
		[[nodiscard]] static VkCommandBufferAllocateInfo CreateBufferDefaultInfo(App& app);
	};
}