#pragma once
#include "Graphics/RenderConventions.h"

namespace game
{
	struct Transform final
	{
		// In pixels.
		glm::vec2 position{};
		float rotation{};
		// In pixels.
		float scale = renderConventions::ENTITY_SIZE;
	};
}
