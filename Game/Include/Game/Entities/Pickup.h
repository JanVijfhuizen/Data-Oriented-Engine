#pragma once
#include "VkEngine/Components/Transform.h"

namespace game
{
	struct Pickup final
	{
		size_t cardId = SIZE_MAX;
		vke::Transform transform{};
		size_t collisionTaskId = SIZE_MAX;
		size_t mouseTaskId = SIZE_MAX;
	};
}
