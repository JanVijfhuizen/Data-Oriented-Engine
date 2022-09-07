#include "pch.h"
#include "Entities/Entity.h"

namespace game
{
	bool EntityId::operator==(const EntityId& other) const
	{
		return id == other.id;
	}

	bool EntityId::operator!=(const EntityId& other) const
	{
		return id != other.id;
	}

	EntityId::operator bool() const
	{
		return id != SIZE_MAX;
	}
}
