#pragma once
#include "Components/Inventory.h"

namespace game
{
	struct EntityData final
	{
		union
		{
			struct Character final
			{
				Inventory inventory{};
			} character{};

			struct Pickup final
			{
				size_t cardId = SIZE_MAX;
			} pickup;
		};
		
		bool markedForDelete = false;
	};

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
		EntityData data{};
		EntityId id{};
	};
}
