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

	struct Entity
	{
		EntityData data{};
		size_t entityTaskId = SIZE_MAX;
	};
}
