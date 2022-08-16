#pragma once
#include "Entity.h"
#include "Components/MovementComponent.h"
#include "Components/PickupComponent.h"

namespace game
{
	struct Character : Entity
	{
		struct Input final
		{
			glm::ivec2 movementDir{};
		};

		MovementComponent movementComponent{};
		PickupComponent pickupComponent{};

		Input input{};

		size_t collisionTaskId = SIZE_MAX;
		size_t mouseTaskId = SIZE_MAX;
		size_t movementTaskId = SIZE_MAX;
		size_t movementTileReservation = SIZE_MAX;
	};
}
