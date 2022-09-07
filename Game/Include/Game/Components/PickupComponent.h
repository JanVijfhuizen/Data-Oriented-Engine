#pragma once
#include "Entities/Entity.h"

namespace game
{
	struct PickupComponent final
	{
		friend class PickupSystem;

		EntityId inInstanceId{};
		EntityId inPickupId{};

		glm::vec2 outHandPosition{};
		bool active = false;

	private:
		glm::vec2 _instancePosition{};
		glm::vec2 _pickupPosition{};
	};
}