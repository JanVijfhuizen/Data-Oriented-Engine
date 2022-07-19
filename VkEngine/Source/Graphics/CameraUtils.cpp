#include "VkEngine/pch.h"
#include "VkEngine/Graphics/CameraUtils.h"
#include "glm/geometric.hpp"

namespace vke
{
	bool Culls(const glm::vec2& camPos, const float pixelSize, const glm::vec2& pos, const glm::vec2& scale)
	{
		glm::vec2 offset = abs(pos - camPos);
		offset -= scale * .5f;
		offset *= pixelSize;
		return offset.x > 2 || offset.y > 2;
	}
}
