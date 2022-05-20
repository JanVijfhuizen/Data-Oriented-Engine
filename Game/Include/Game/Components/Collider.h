#pragma once

namespace game
{
	struct Collider final
	{
		bool moveable = false;
		glm::vec2 delta{};
	};
}