#pragma once
#include "StackArray.h"
#include "Components/MovementComponent.h"
#include "Systems/MenuSystem.h"
#include "VkEngine/Components/Transform.h"

namespace game::demo
{
	struct DummyEntity final
	{
		MovementComponent movementComponent{};
		vke::Transform transform{};

		MenuUpdateInfo menuUpdateInfo{};
		jlb::StackArray<size_t, 2> menuInteractIds{};

		size_t movementTaskId = SIZE_MAX;
		size_t collisionTaskId = SIZE_MAX;
		size_t movementTileReservation = SIZE_MAX;
	};
}
