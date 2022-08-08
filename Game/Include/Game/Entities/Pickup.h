#pragma once
#include "Entity.h"
#include "VkEngine/Components/Transform.h"

namespace game
{
	struct Pickup final
	{
		Entity entity{};
		size_t cardId = SIZE_MAX;
		vke::Transform transform{};

		size_t collisionTaskId = SIZE_MAX;
		size_t entityTaskId = SIZE_MAX;
		size_t mouseTaskId = SIZE_MAX;
	};
}
