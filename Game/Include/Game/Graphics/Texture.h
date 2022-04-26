#pragma once
#include "VkRenderer/VkMemBlock.h"

namespace game
{
	struct Texture final
	{
		VkImage image;
		vk::MemBlock memBlock;
		glm::ivec2 resolution;
	};
}
