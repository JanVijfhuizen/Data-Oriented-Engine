#pragma once

namespace game
{
	struct Collider final
	{
		enum class Shape
		{
			Box,
			Circle
		} shape;
		glm::vec2 delta;
	};
}