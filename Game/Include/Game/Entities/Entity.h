#pragma once
#include "VkEngine/Components/Transform.h"

namespace game
{
	struct EntityId final
	{
		size_t index = SIZE_MAX;
		size_t id = SIZE_MAX;

		[[nodiscard]] bool operator==(const EntityId& other) const;
		[[nodiscard]] bool operator!=(const EntityId& other) const;
		[[nodiscard]] operator bool() const;
	};

	struct Entity
	{
		EntityId id{};
		vke::Transform transform{};
		bool markedForDelete = false;
	};
}
