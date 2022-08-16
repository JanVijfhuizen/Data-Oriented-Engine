#pragma once
#include "Entities/Entity.h"

namespace game
{
	struct PickupComponent final
	{
		friend class PickupSystem;

		EntityId instance{};
		EntityId pickup{};

		glm::vec2 handPositions[2]{};
		bool valid = true;

	private:
		glm::vec2 _instancePosition{};
		glm::vec2 _pickupPosition{};
	};
}
