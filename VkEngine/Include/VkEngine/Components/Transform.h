#pragma once
#include "VkEngine/Graphics/RenderConventions.h"

namespace vke
{
	struct Transform final
	{
		// In pixels.
		glm::vec2 position{};
		// In radians.
		float rotation = 0;
		// In pixels.
		float scale = PIXEL_SIZE_ENTITY;
	};
}
