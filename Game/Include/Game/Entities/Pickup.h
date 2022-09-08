#pragma once
#include "Entity.h"

namespace game
{
	struct Pickup final : Entity
	{
		size_t cardId = SIZE_MAX;
		size_t collisionJobId = SIZE_MAX;
		size_t mouseJobId = SIZE_MAX;
		bool interacted = false;
	};
}
