#pragma once
#include "Components/MovementComponent.h"
#include "VkEngine/Components/Transform.h"

namespace game
{
	struct Player final
	{
		MovementComponent movementComponent{};
		vke::Transform transform{};

		size_t movementTaskId = SIZE_MAX;
		size_t collisionTaskId = SIZE_MAX;
		size_t movementTileReservation = SIZE_MAX;
	};
}
