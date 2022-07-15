#pragma once

namespace game
{
	struct MovementComponent final
	{
		// Settings.
		float scaleMultiplier = 1;
		float bobbingAmount = 8;

		// Set at start of tick.
		float rotation;
		glm::vec2 from{};
		glm::vec2 to{};

		// System data.
		size_t remaining = 0;
	};
}