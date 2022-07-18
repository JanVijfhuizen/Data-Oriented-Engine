#pragma once

namespace game
{
	// Component related to the movement system.
	struct MovementComponent final
	{
		struct Settings final
		{
			float bobbingAmount = 2;
		} settings;

		struct UserDefinedOnTick final
		{
			glm::vec2 from{};
			glm::vec2 to{};
			float rotation;
			size_t remaining = 0;
		} userDefined;
		
		struct SystemDefined final
		{
			float scaleMultiplier = 1;
		} systemDefined;
	};
}