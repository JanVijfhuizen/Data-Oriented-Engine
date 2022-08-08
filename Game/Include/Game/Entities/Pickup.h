#pragma once
#include "VkEngine/Components/Transform.h"

namespace game
{
	struct Pickup final
	{
		vke::Transform transform{};
		size_t collisionTaskId = SIZE_MAX;
		size_t mouseTaskId = SIZE_MAX;
	};
}
