#pragma once
#include "StackArray.h"
#include "Components/MovementComponent.h"
#include "Systems/MenuSystem.h"
#include "VkEngine/Components/Transform.h"

namespace game
{
	struct Player final
	{
		MovementComponent movementComponent{};
		vke::Transform transform{};

		MenuUpdateInfo menuUpdateInfo{};
		jlb::StackArray<size_t, 5> menuInteractIds{};

		size_t movementTaskId = SIZE_MAX;
		size_t collisionTaskId = SIZE_MAX;
		size_t movementTileReservation = SIZE_MAX;
	};
}
