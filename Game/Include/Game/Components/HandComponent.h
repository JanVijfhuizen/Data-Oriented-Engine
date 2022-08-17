#pragma once

namespace game
{
	struct HandComponent final
	{
		struct Hand final
		{
			glm::vec2 position{};
			glm::vec2 target{};
			glm::vec2 anchor{};
			float range = 1;
		};

		Hand hands[2]{};
	};
}