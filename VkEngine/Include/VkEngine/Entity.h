#pragma once

namespace vke
{
	struct Entity final
	{
		size_t id = SIZE_MAX;
		bool markedForDelete = false;
	};
}
