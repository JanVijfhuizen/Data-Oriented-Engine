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

		enum class Type
		{
			character,
			pickup
		} type = Type::character;
		
		bool markedForDelete = false;
	};

	struct EntityId final
	{
		size_t index = SIZE_MAX;
		size_t id = SIZE_MAX;
	};

	struct Entity
	{
		EntityData data{};
		EntityId id{};
	};
}
