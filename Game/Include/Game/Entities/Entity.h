#pragma once

namespace game
{
	struct Entity final
	{
		size_t id = SIZE_MAX;
		bool markedForDelete = false;
	};
}
