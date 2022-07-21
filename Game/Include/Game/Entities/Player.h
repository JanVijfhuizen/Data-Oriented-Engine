#pragma once
#include "Components/MovementComponent.h"
#include "VkEngine/Components/Transform.h"

namespace game
{
	struct Player final
	{
		MovementComponent movementComponent{};
		vke::Transform transform{};

		size_t movementTaskId;
		size_t collisionTaskId;
	};
}
