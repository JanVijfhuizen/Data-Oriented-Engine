#pragma once

namespace game
{
	struct Transform final
	{
		glm::vec2 position{};
		float rotation{};
		float scale{ 1 };
	};
}