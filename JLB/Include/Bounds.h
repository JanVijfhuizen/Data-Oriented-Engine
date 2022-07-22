#pragma once
#include <glm/vec2.hpp>

namespace jlb
{
	struct Bounds final
	{
		glm::ivec2 lBot{};
		glm::ivec2 rTop{};

		Bounds();
		Bounds(const glm::ivec2& position);

		[[nodiscard]] glm::ivec2 GetCenter() const;
	};
}
