#pragma once
#include "Entity.h"

namespace game
{
	struct Pickup final : Entity
	{
		size_t collisionTaskId = SIZE_MAX;
		size_t mouseTaskId = SIZE_MAX;
		bool interacted = false;
	};
}
