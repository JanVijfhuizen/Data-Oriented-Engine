#pragma once
#include "Entity.h"
#include "Components/MovementComponent.h"
#include "VkEngine/Components/Transform.h"

namespace game
{
	struct Character : Entity
	{
		vke::Transform transform{};
		MovementComponent movementComponent{};

		size_t collisionTaskId = SIZE_MAX;
		size_t mouseTaskId = SIZE_MAX;
		size_t movementTaskId = SIZE_MAX;
		size_t movementTileReservation = SIZE_MAX;
	};
}
