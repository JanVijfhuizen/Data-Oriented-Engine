#pragma once
#include "VkRenderer/VkMemBlock.h"

namespace game
{
	struct Buffer final
	{
		VkBuffer buffer;
		vk::MemBlock memBlock;
	};
}
