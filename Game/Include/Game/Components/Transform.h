#pragma once

namespace game
{
	struct Transform final
	{
		glm::vec2 position{0};
		float rotation = 0;
		float scale = 1;
	};
}