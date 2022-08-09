#pragma once
#include "Entity.h"
#include "VkEngine/Components/Transform.h"

namespace game
{
	struct Pickup final : Entity
	{
		vke::Transform transform{};

		size_t collisionTaskId = SIZE_MAX;
		size_t mouseTaskId = SIZE_MAX;
		bool interacted = false;
	};
}