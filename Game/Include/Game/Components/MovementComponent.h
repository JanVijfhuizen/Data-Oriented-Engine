#pragma once

namespace game
{
	struct MovementComponent final
	{
		glm::vec2 from{};
		glm::vec2 to{};
		size_t remaining = 0;
		float scaleMultiplier = 1;
		float bobbingAmount = 8;
	};
}