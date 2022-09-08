#pragma once
#include "Entity.h"
#include "Components/MovementComponent.h"
#include "Components/PickupComponent.h"
#include <Pile.h>
#include "Components/Inventory.h"

namespace game
{
	struct Character : Entity
	{
		struct Input final
		{
			glm::ivec2 movementDir{};
		};

		Inventory inventory{};

		MovementComponent movementComponent{};
		PickupComponent pickupComponent{};

		jlb::Pile<glm::vec2> lHandPosPile{};
		jlb::Pile<glm::vec2> rHandPosPile{};
		glm::vec2 lHandPos{};
		glm::vec2 rHandPos{};
		float headRotation{};

		Input input{};

		size_t collisionJobId = SIZE_MAX;
		size_t mouseJobId = SIZE_MAX;
		size_t movementJobId = SIZE_MAX;
		size_t movementTileReservation = SIZE_MAX;
		size_t pickupJobId = SIZE_MAX;
	};
}
