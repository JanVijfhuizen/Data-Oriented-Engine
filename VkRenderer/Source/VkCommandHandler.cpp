#include "pch.h"
#include "VkCommandHandler.h"
#include "VkApp.h"

namespace vk
{
	VkCommandBufferAllocateInfo CommandHandler::CreateBufferDefaultInfo(App& app)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = app.commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;
		return allocInfo;
	}
}
