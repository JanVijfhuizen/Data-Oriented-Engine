#pragma once
#include "VkRenderer/VkMemBlock.h"

namespace vke
{
	struct Buffer final
	{
		VkBuffer buffer{};
		vk::MemBlock memBlock{};
	};
}
