#pragma once

namespace game
{
	struct MovementTaskOutput;

	// Component related to the movement system.
	struct MovementComponent final
	{
		float bobbingAmount = 2;

		glm::vec2 from{};
		glm::vec2 to{};
		glm::vec2 position{};
		float rotation;
		size_t duration = 1;

		float scaleMultiplier = 1;
		size_t remaining = SIZE_MAX;
	};
}