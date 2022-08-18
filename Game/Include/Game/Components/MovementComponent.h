#pragma once

namespace game
{
	struct MovementComponent final
	{
		glm::vec2 inFrom{};
		glm::vec2 inTo{};
		size_t inDuration = 1;
		
		glm::vec2 outPosition;
		float outRotation;
		float outScaleMultiplier = 1;
		size_t outRemaining;

		bool active = false;
	};
}