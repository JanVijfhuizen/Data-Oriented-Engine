#pragma once

namespace game
{
	struct EntityData final
	{
		bool markedForDelete = false;
	};

	struct Entity
	{
		EntityData data{};
		size_t entityTaskId = SIZE_MAX;
	};
}