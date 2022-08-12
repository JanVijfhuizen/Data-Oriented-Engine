#pragma once
#include "VkEngine/Systems/TaskSystemWithOutput.h"

namespace game
{
	struct PickupTask final
	{
		
	};

	struct PickupTaskOutput final
	{
		size_t item = SIZE_MAX;
	};

	class PickupSystem : public vke::TaskSystemWithOutput<PickupTask, PickupTaskOutput>
	{
	public:

	};
}

