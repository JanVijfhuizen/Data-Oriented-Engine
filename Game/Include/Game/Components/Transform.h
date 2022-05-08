#pragma once

namespace game
{
	struct alignas(32) Transform final
	{
		glm::vec2 position{};
		glm::vec2 scale{16};
		float rotation{};
	};
}