#pragma once
#include "VkRenderer/VkMemBlock.h"

namespace game
{
	struct Mesh final
	{
		VkBuffer vertexBuffer;
		VkBuffer indexBuffer;
		vk::MemBlock vertexMemBlock;
		vk::MemBlock indexMemBlock;
		size_t indexCount;
	};
}
