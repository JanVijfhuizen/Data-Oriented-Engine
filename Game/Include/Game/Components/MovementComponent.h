#pragma once

namespace game
{
	struct MovementComponent final
	{
		glm::vec2 inFrom{};
		glm::vec2 inTo{};
		size_t inDuration = 1;
		bool inIsObstructed = false;
		
		glm::vec2 outPosition;
		float outRotation;
		float outScaleMultiplier = 1;

		bool active = false;
		size_t remaining;
	};
}