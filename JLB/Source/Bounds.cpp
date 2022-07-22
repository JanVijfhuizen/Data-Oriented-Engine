#include "Bounds.h"

namespace jlb
{
	Bounds::Bounds() = default;

	Bounds::Bounds(const glm::ivec2& position) : lBot(position), rTop(position)
	{

	}

	glm::ivec2 Bounds::GetCenter() const
	{
		return lBot + (rTop - lBot) / 2;
	}
}