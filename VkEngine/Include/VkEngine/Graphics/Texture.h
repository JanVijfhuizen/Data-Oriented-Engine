#pragma once
#include "VkRenderer/VkMemBlock.h"

namespace vke
{
	struct Texture final
	{
		VkImage image{};
		vk::MemBlock memBlock{};
		glm::ivec2 resolution{};
	};
}
