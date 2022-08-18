#pragma once
#include "Entity.h"
#include "Components/MovementComponent.h"
#include "Components/PickupComponent.h"
#include <Pile.h>

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

		jlb::Pile<glm::vec2> lHandPos{};
		jlb::Pile<glm::vec2> rHandPos{};

		Input input{};

		size_t collisionTaskId = SIZE_MAX;
		size_t mouseTaskId = SIZE_MAX;
		size_t movementTaskId = SIZE_MAX;
		size_t movementTileReservation = SIZE_MAX;
		size_t pickupTaskId = SIZE_MAX;
	};
}
